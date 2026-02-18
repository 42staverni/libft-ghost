<script lang="ts">
	import DropZone from "$lib/components/DropZone.svelte";
	import FunctionPicker from "$lib/components/FunctionPicker.svelte";
	import ProgressBar from "$lib/components/ProgressBar.svelte";
	import TestReport from "$lib/components/TestReport.svelte";
	import {
		uploadProject,
		runTests,
		type FunctionInfo,
		type TestResult,
		type ProgressEvent,
		type Summary,
		type CompileInfo,
		type NorminetteResult,
	} from "$lib/api";

	type AppState = "upload" | "select" | "testing" | "results";

	let appState: AppState = $state("upload");
	let error: string | null = $state(null);
	let sessionId = $state("");
	let functions: FunctionInfo[] = $state([]);
	let selectedFunctions: string[] = $state([]);
	let progress: ProgressEvent | null = $state(null);
	let results: TestResult[] = $state([]);
	let summary: Summary | null = $state(null);
	let uploading = $state(false);
	let makeInfo: CompileInfo | null = $state(null);
	let abortRunningTests: (() => void) | null = $state(null);
	let runMemoryChecks = $state(false);
	let norminette: NorminetteResult | null = $state(null);

	$effect(() => {
		return () => {
			abortRunningTests?.();
		};
	});

	async function handleFileSelect(file: File) {
		error = null;
		uploading = true;
		try {
			const res = await uploadProject(file);
			sessionId = res.session_id;
			functions = res.functions;
			selectedFunctions = res.functions.map((f) => f.name);
			norminette = res.norminette ?? null;
			appState = "select";
		} catch (e) {
			error = e instanceof Error ? e.message : "Upload failed";
		} finally {
			uploading = false;
		}
	}

	function handleRun() {
		abortRunningTests?.();
		abortRunningTests = null;

		appState = "testing";
		results = [];
		summary = null;
		makeInfo = null;
		error = null;

		abortRunningTests = runTests(
			sessionId,
			selectedFunctions,
			runMemoryChecks,
			(evt) => {
				progress = evt;
			},
			(result) => {
				results = [...results, result];
			},
			(report) => {
				summary = report.summary;
				results = report.results;
				appState = "results";
				progress = null;
				abortRunningTests = null;
			},
			(err) => {
				error = typeof err === "string" ? err : "An error occurred";
				appState = "results";
				progress = null;
				abortRunningTests = null;
			},
			(info) => {
				makeInfo = info;
			},
		);
	}

	function reset() {
		abortRunningTests?.();
		abortRunningTests = null;

		appState = "upload";
		error = null;
		sessionId = "";
		functions = [];
		selectedFunctions = [];
		progress = null;
		results = [];
		summary = null;
		makeInfo = null;
		runMemoryChecks = false;
	}

	function cancel() {
		abortRunningTests?.();
		abortRunningTests = null;

		appState = "upload";
		error = null;
		sessionId = "";
		functions = [];
		selectedFunctions = [];
		runMemoryChecks = true;
	}
</script>

<div class="min-h-screen flex flex-col">
	<!-- Hero Section -->
	<div class="flex-1 flex flex-col items-center justify-center px-6 py-16">
		<!-- Logo/Title -->
		<div class="text-center mb-12 animate-slide-up">
			<div class="inline-flex items-baseline gap-3">
				<span class="text-7xl font-light text-accent-blue tracking-tight">libft</span>
				<h1 class="text-5xl font-bold tracking-tight text-ink ghost-text">ghost</h1>
			</div>
			<p class="text-ink-muted text-lg mt-4 font-light">
				👻 Watching over your code
			</p>
		</div>

		<!-- Main Content Area -->
		<div class="w-full max-w-2xl">
			<!-- Error Message -->
			{#if error}
				<div class="mb-6 animate-slide-up" style="animation-delay: 0.1s">
					<div class="glass rounded-2xl border border-accent-rose-soft bg-accent-rose-soft/30 p-4 flex items-start gap-3">
						<svg class="w-5 h-5 text-accent-rose flex-shrink-0 mt-0.5" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
							<path stroke-linecap="round" stroke-linejoin="round" d="M6 18L18 6M6 6l12 12" />
						</svg>
						<span class="text-ink text-sm">{error}</span>
					</div>
				</div>
			{/if}

			<!-- Upload State -->
			{#if appState === "upload"}
				<div class="animate-slide-up" style="animation-delay: 0.1s">
					<DropZone onFileSelect={handleFileSelect} {uploading} />
				</div>
			{/if}

			<!-- Select Functions State -->
			{#if appState === "select"}
				<div class="animate-scale-in">
					<div class="glass rounded-3xl p-1 mb-4">
						<label class="flex items-center justify-between gap-4 p-4 cursor-pointer">
							<div>
								<p class="text-sm font-medium text-ink">Memory diagnostics</p>
								<p class="text-xs text-ink-faint mt-0.5">
									Use Valgrind + AddressSanitizer for thorough testing
								</p>
							</div>
							<div class="relative">
								<input
									type="checkbox"
									class="peer sr-only"
									bind:checked={runMemoryChecks}
								/>
								<div class="w-11 h-6 bg-surface-sunken rounded-full peer peer-checked:bg-accent-blue transition-colors"></div>
								<div class="absolute left-1 top-1 w-4 h-4 bg-white rounded-full transition-transform peer-checked:translate-x-5"></div>
							</div>
						</label>
					</div>
					<FunctionPicker
						{functions}
						{norminette}
						bind:selected={selectedFunctions}
						onRun={handleRun}
						onCancel={cancel}
					/>
				</div>
			{/if}

			<!-- Testing State -->
			{#if appState === "testing"}
				<div class="animate-scale-in space-y-4">
					{#if makeInfo}
						<div class="glass rounded-2xl p-5 {makeInfo.success ? 'border-l-4 border-accent-emerald' : 'border-l-4 border-accent-rose'}">
							<div class="flex items-center gap-3">
								<div class="w-8 h-8 rounded-full flex items-center justify-center {makeInfo.success ? 'bg-accent-emerald-soft' : 'bg-accent-rose-soft'}">
									<span class="text-lg">{makeInfo.success ? "✓" : "✗"}</span>
								</div>
								<div class="flex-1">
									<p class="font-medium text-ink">
										{makeInfo.success ? "make — libft.a built successfully" : "make failed"}
									</p>
								</div>
							</div>
							{#if makeInfo.output}
								<details class="mt-3">
									<summary class="cursor-pointer text-xs text-ink-faint hover:text-ink-muted font-medium">Build output</summary>
									<pre class="mt-2 whitespace-pre-wrap text-xs text-ink-muted max-h-48 overflow-y-auto font-mono rounded-xl p-3 bg-surface-sunken">{makeInfo.output}</pre>
								</details>
							{/if}
						</div>
					{:else if !progress}
						<div class="glass rounded-2xl p-6 text-center">
							<div class="inline-flex items-center gap-3">
								<div class="w-2 h-2 bg-accent-blue rounded-full animate-pulse"></div>
								<span class="text-sm text-ink-muted">Running make...</span>
							</div>
						</div>
					{/if}
					<ProgressBar {progress} />
					{#if results.length > 0}
						<div class="mt-6">
							<TestReport {results} summary={null} />
						</div>
					{/if}
				</div>
			{/if}

			<!-- Results State -->
			{#if appState === "results"}
				<div class="animate-scale-in">
					<div class="glass rounded-3xl p-6">
						<div class="flex items-center justify-between mb-6">
							<div class="flex items-center gap-3">
								<h2 class="text-2xl font-bold text-ink">Results</h2>
								<span class="rounded-full px-3 py-1 text-xs font-medium {runMemoryChecks ? 'bg-accent-emerald-soft text-accent-emerald' : 'bg-accent-amber-soft text-accent-amber'}">
									{runMemoryChecks ? "Full diagnostics" : "Quick mode"}
								</span>
							</div>
							<button
								class="glass-button rounded-full px-5 py-2 text-sm font-medium text-ink"
								onclick={reset}
							>
								Test another
							</button>
						</div>
						
						{#if makeInfo}
							<div class="glass rounded-2xl p-4 mb-4 {makeInfo.success ? 'border-l-4 border-accent-emerald' : 'border-l-4 border-accent-rose'}">
								<div class="flex items-center gap-3">
									<div class="w-8 h-8 rounded-full flex items-center justify-center {makeInfo.success ? 'bg-accent-emerald-soft' : 'bg-accent-rose-soft'}">
										<span class="text-lg">{makeInfo.success ? "✓" : "✗"}</span>
									</div>
									<p class="font-medium text-ink">
										{makeInfo.success ? "make — libft.a built successfully" : "make failed — fix your Makefile first"}
									</p>
								</div>
								{#if makeInfo.output && !makeInfo.success}
									<pre class="mt-3 max-h-48 overflow-y-auto whitespace-pre-wrap rounded-xl p-3 font-mono text-xs bg-surface-sunken">{makeInfo.output}</pre>
								{/if}
							</div>
						{/if}
						
						<TestReport {results} {summary} />
					</div>
				</div>
			{/if}
		</div>
	</div>

	<!-- Footer -->
	<footer class="py-8 text-center">
		<div class="inline-flex items-center gap-3 text-ink-faint text-sm">
			<div class="w-8 h-px bg-border-soft"></div>
			<span>Built for 42 students</span>
			<div class="w-8 h-px bg-border-soft"></div>
		</div>
	</footer>
</div>

<style>
	.ghost-text {
		text-shadow: 
			0 0 10px rgba(255, 255, 255, 0.4),
			0 0 20px rgba(255, 255, 255, 0.2),
			0 0 30px rgba(255, 255, 255, 0.1);
		filter: blur(0.3px);
		position: relative;
	}
	
	.ghost-text::after {
		content: 'ghost';
		position: absolute;
		left: 0;
		top: 0;
		width: 100%;
		height: 100%;
		background: linear-gradient(
			90deg,
			transparent 0%,
			rgba(255, 255, 255, 0.3) 50%,
			transparent 100%
		);
		background-size: 200% 100%;
		animation: ghost-fog 3s ease-in-out infinite;
		-webkit-background-clip: text;
		background-clip: text;
		pointer-events: none;
	}
	
	@keyframes ghost-fog {
		0%, 100% {
			background-position: 200% 0;
		}
		50% {
			background-position: -200% 0;
		}
	}
</style>
