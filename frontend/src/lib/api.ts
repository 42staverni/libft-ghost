// API configuration
// Use relative URL for production (same origin), fallback to localhost:8080 for dev
const API_URL = import.meta.env.VITE_API_URL || '';

export interface FunctionInfo {
	name: string;
	filename: string;
	is_bonus: boolean;
	norminette_errors?: number;
	norminette_output?: string;
}

export interface NorminetteError {
	file: string;
	line: number;
	column: number;
	message: string;
	rule?: string;
}

export interface NorminetteResult {
	has_errors: boolean;
	total_errors: number;
	errors: NorminetteError[];
	errors_by_file: Record<string, NorminetteError[]>;
}

export interface DetectResponse {
	session_id: string;
	functions: FunctionInfo[];
	has_bonus: boolean;
	norminette?: NorminetteResult;
}

export interface TestCase {
	name: string;
	passed: boolean;
	expected: string;
	got: string;
	info?: string;
	source?: string;
}

export interface CompileInfo {
	success: boolean;
	output: string;
	command: string;
}

export interface MemoryCheck {
	has_leaks: boolean;
	leak_summary: string;
	bytes_lost: number;
	blocks_lost: number;
	error_count: number;
	raw_output: string;
}

export interface Sanitizer {
	has_errors: boolean;
	errors: string[];
	raw_output: string;
}

export interface Protection {
	null_input?: boolean;
	zero_len?: boolean;
	int_overflow?: boolean;
	empty_string?: boolean;
	malloc_fail_first?: boolean;
	malloc_fail_mid?: boolean;
}

export interface NorminetteCheck {
	has_errors: boolean;
	errors: NorminetteError[];
}

export interface TestResult {
	function: string;
	status: 'pass' | 'fail' | 'error';
	compilation: CompileInfo;
	tests: TestCase[];
	memory_check: MemoryCheck;
	sanitizer_check: Sanitizer;
	protection: Protection;
	norminette_check: NorminetteCheck;
	hints?: string[];
	duration: string;
}

export interface ProgressEvent {
	function: string;
	step: string;
	current: number;
	total: number;
}

export interface Summary {
	total: number;
	passed: number;
	failed: number;
	errors: number;
	leak_count: number;
	sanitizer_hits: number;
}

export interface Report {
	session_id: string;
	timestamp: string;
	results: TestResult[];
	summary: Summary;
}

export async function uploadProject(file: File): Promise<DetectResponse> {
	const formData = new FormData();
	formData.append('project', file);

	const res = await fetch(`${API_URL}/api/upload`, {
		method: 'POST',
		body: formData,
	});

	if (!res.ok) {
		throw new Error(await res.text());
	}

	return res.json();
}

export function runTests(
	sessionId: string,
	functions: string[],
	runMemoryChecks: boolean,
	onProgress: (event: ProgressEvent) => void,
	onResult: (result: TestResult) => void,
	onDone: (report: Report) => void,
	onError: (error: string) => void,
	onMake?: (info: CompileInfo) => void,
): () => void {
	const body = JSON.stringify({
		session_id: sessionId,
		functions,
		run_memory_checks: runMemoryChecks,
	});

	// Use fetch + ReadableStream for SSE via POST
	const controller = new AbortController();

	fetch(`${API_URL}/api/test`, {
		method: 'POST',
		headers: { 'Content-Type': 'application/json' },
		body,
		signal: controller.signal,
	})
		.then(async (res) => {
			if (!res.ok) {
				onError(await res.text());
				return;
			}
			const reader = res.body!.getReader();
			const decoder = new TextDecoder();
			let buffer = '';
			let receivedTerminalEvent = false;

			const processEventBlock = (block: string) => {
				const lines = block.split('\n');
				let currentEvent = '';
				const dataLines: string[] = [];

				for (const rawLine of lines) {
					const line = rawLine.trimEnd();
					if (!line) continue;
					if (line.startsWith('event:')) {
						currentEvent = line.slice(6).trim();
					} else if (line.startsWith('data:')) {
						dataLines.push(line.slice(5).trimStart());
					}
				}

				if (!currentEvent || dataLines.length === 0) {
					return;
				}

				let data: unknown;
				try {
					data = JSON.parse(dataLines.join('\n'));
				} catch {
					return;
				}

				switch (currentEvent) {
					case 'progress':
						onProgress(data as ProgressEvent);
						break;
					case 'make':
						onMake?.(data as CompileInfo);
						break;
					case 'result':
						onResult(data as TestResult);
						break;
					case 'done':
						receivedTerminalEvent = true;
						onDone(data as Report);
						break;
					case 'error':
						receivedTerminalEvent = true;
						onError(typeof data === 'string' ? data : 'An error occurred');
						break;
				}
			};

			while (true) {
				const { done, value } = await reader.read();
				if (done) {
					buffer += decoder.decode();
					break;
				}

				buffer += decoder.decode(value, { stream: true });
				const blocks = buffer.split('\n\n');
				buffer = blocks.pop() || '';
				for (const block of blocks) {
					processEventBlock(block);
				}
			}

			if (buffer.trim().length > 0) {
				processEventBlock(buffer);
			}

			if (!receivedTerminalEvent) {
				onError('Test stream ended unexpectedly before completion');
			}
		})
		.catch((err) => {
			if (err.name !== 'AbortError') {
				onError(err.message);
			}
		});

	return () => controller.abort();
}
