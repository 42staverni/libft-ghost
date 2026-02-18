<script lang="ts">
	import type { TestResult, Summary } from "$lib/api";
	import {
		PART_ORDER,
		getFunctionPart,
		type LibftPart,
	} from "$lib/libftParts";

	let {
		results,
		summary,
	}: {
		results: TestResult[];
		summary: Summary | null;
	} = $props();

	let expandedFunc = $state<string | null>(null);

	function statusColor(status: string): string {
		switch (status) {
			case "pass":
				return "text-accent-emerald";
			case "fail":
				return "text-accent-rose";
			default:
				return "text-accent-amber";
		}
	}

	function statusBorder(status: string): string {
		switch (status) {
			case "pass":
				return "border-l-4 border-accent-emerald";
			case "fail":
				return "border-l-4 border-accent-rose";
			default:
				return "border-l-4 border-accent-amber";
		}
	}

	function getResultsByPart(part: LibftPart): TestResult[] {
		return results.filter((result) => getFunctionPart(result.function) === part);
	}

	function statusIcon(status: string): string {
		switch (status) {
			case "pass":
				return "✓";
			case "fail":
				return "✗";
			default:
				return "⚠";
		}
	}

	function formatDuration(duration: string): string {
		if (!duration) return "0.0s";

		const unitMatches = Array.from(
			duration.matchAll(/([\d.]+)(ns|µs|us|ms|s|m|h)/g),
		);

		if (unitMatches.length === 0) {
			return duration;
		}

		let totalMs = 0;
		for (const match of unitMatches) {
			const value = parseFloat(match[1]);
			const unit = match[2];
			if (Number.isNaN(value)) continue;

			switch (unit) {
				case "h":
					totalMs += value * 60 * 60 * 1000;
					break;
				case "m":
					totalMs += value * 60 * 1000;
					break;
				case "s":
					totalMs += value * 1000;
					break;
				case "ms":
					totalMs += value;
					break;
				case "us":
				case "µs":
					totalMs += value / 1000;
					break;
				case "ns":
					totalMs += value / 1_000_000;
					break;
			}
		}

		if (totalMs >= 1000) {
			return (totalMs / 1000).toFixed(1) + "s";
		}

		if (totalMs >= 1) {
			return totalMs.toFixed(0) + "ms";
		}

		return "<1ms";
	}

	function memoryHasNoLeaks(result: TestResult): boolean {
		const memoryCheck = result.memory_check;
		if (!memoryCheck || memoryCheck.has_leaks) return false;

		const summary = memoryCheck.leak_summary?.toLowerCase() ?? "";
		return summary.includes("no leak") || summary === "";
	}

	const partColors: Record<LibftPart, string> = {
		"Part 1": "bg-accent-blue",
		"Part 2": "bg-accent-purple",
		"Part 3": "bg-accent-emerald",
	};
</script>

{#if summary}
	<div class="grid grid-cols-2 sm:grid-cols-3 lg:grid-cols-6 gap-3 mb-8">
		<div class="glass rounded-2xl p-4 text-center">
			<div class="text-2xl font-bold text-ink">{summary.total}</div>
			<div class="text-xs font-medium text-ink-faint uppercase tracking-wider mt-1">Total</div>
		</div>
		<div class="glass rounded-2xl p-4 text-center border-accent-emerald/30">
			<div class="text-2xl font-bold text-accent-emerald">{summary.passed}</div>
			<div class="text-xs font-medium text-accent-emerald/70 uppercase tracking-wider mt-1">Passed</div>
		</div>
		<div class="glass rounded-2xl p-4 text-center border-accent-rose/30">
			<div class="text-2xl font-bold text-accent-rose">{summary.failed}</div>
			<div class="text-xs font-medium text-accent-rose/70 uppercase tracking-wider mt-1">Failed</div>
		</div>
		<div class="glass rounded-2xl p-4 text-center border-accent-amber/30">
			<div class="text-2xl font-bold text-accent-amber">{summary.errors}</div>
			<div class="text-xs font-medium text-accent-amber/70 uppercase tracking-wider mt-1">Errors</div>
		</div>
		<div class="glass rounded-2xl p-4 text-center">
			<div class="text-2xl font-bold text-accent-amber">{summary.leak_count}</div>
			<div class="text-xs font-medium text-accent-amber/70 uppercase tracking-wider mt-1">Leaks</div>
		</div>
		<div class="glass rounded-2xl p-4 text-center border-accent-rose/30">
			<div class="text-2xl font-bold text-accent-rose">{summary.sanitizer_hits}</div>
			<div class="text-xs font-medium text-accent-rose/70 uppercase tracking-wider mt-1">UB/OOB</div>
		</div>
	</div>
{/if}

<div class="space-y-8">
	{#each PART_ORDER as part}
		{@const partResults = getResultsByPart(part)}
		{@const partColor = partColors[part]}
		{#if partResults.length > 0}
			<section>
				<div class="flex items-center gap-3 mb-4">
					<div class="w-2 h-2 rounded-full {partColor}"></div>
					<h3 class="text-xs font-semibold text-ink-faint uppercase tracking-widest">{part}</h3>
					<div class="flex-1 h-px bg-border-soft"></div>
					<span class="glass-button rounded-full px-2 py-0.5 text-[11px] font-medium text-ink-faint">
						{partResults.length}
					</span>
				</div>

				<div class="grid gap-3 md:grid-cols-2 xl:grid-cols-3">
					{#each partResults as result, i}
						<article
							class="glass rounded-2xl overflow-hidden transition-all duration-300 {statusBorder(result.status)} {expandedFunc === result.function ? 'md:col-span-2 xl:col-span-3' : ''}"
							style="animation: slide-up 0.4s ease-out {i * 0.03}s both"
						>
							<button
								class="w-full p-4 text-left"
								onclick={() =>
									(expandedFunc =
										expandedFunc === result.function
											? null
											: result.function)}
							>
								<div class="flex items-center justify-between gap-3">
									<div class="flex items-center gap-3 min-w-0">
										<div class="w-8 h-8 rounded-full flex items-center justify-center flex-shrink-0 {result.status === 'pass' ? 'bg-accent-emerald-soft' : result.status === 'fail' ? 'bg-accent-rose-soft' : 'bg-accent-amber-soft'}">
											<span class="text-lg {statusColor(result.status)}">{statusIcon(result.status)}</span>
										</div>
										<span class="truncate font-mono text-sm font-medium text-ink">{result.function}</span>
									</div>
									<svg
										class="w-5 h-5 text-ink-faint flex-shrink-0 transition-transform duration-300 {expandedFunc === result.function ? 'rotate-180' : ''}"
										viewBox="0 0 20 20"
										fill="currentColor"
									>
										<path
											fill-rule="evenodd"
											d="M5.23 7.21a.75.75 0 011.06.02L10 11.168l3.71-3.938a.75.75 0 111.08 1.04l-4.25 4.5a.75.75 0 01-1.08 0l-4.25-4.5a.75.75 0 01.02-1.06z"
											clip-rule="evenodd"
										/>
									</svg>
								</div>

								<div class="flex flex-wrap items-center gap-2 mt-3">
									<span class="glass-button rounded-full px-2 py-0.5 text-[11px] font-medium text-ink-faint">
										{formatDuration(result.duration)}
									</span>
									{#if result.tests}
										<span class="glass-button rounded-full px-2 py-0.5 text-[11px] font-medium text-ink-faint">
											{result.tests.filter((t) => t.passed).length}/{result.tests.length}
										</span>
									{/if}
									{#if result.memory_check?.has_leaks}
										<span class="glass-button rounded-full px-2 py-0.5 text-[11px] font-medium bg-accent-amber-soft text-accent-amber">leak</span>
									{:else if (result.memory_check?.error_count ?? 0) > 0}
										<span class="glass-button rounded-full px-2 py-0.5 text-[11px] font-medium bg-accent-amber-soft text-accent-amber">valgrind</span>
									{/if}
									{#if result.sanitizer_check?.has_errors}
										<span class="glass-button rounded-full px-2 py-0.5 text-[11px] font-medium bg-accent-rose-soft text-accent-rose">sanitizer</span>
									{/if}
									{#if result.norminette_check?.has_errors}
										<span class="glass-button rounded-full px-2 py-0.5 text-[11px] font-medium bg-accent-purple-soft text-accent-purple">norminette</span>
									{/if}
								</div>
							</button>

							{#if expandedFunc === result.function}
								<div class="border-t border-border-soft p-4 space-y-4 bg-surface-sunken/50">
									<!-- Norminette -->
									{#if result.norminette_check?.has_errors}
										<div class="glass rounded-xl p-3 border-accent-purple/30">
											<h4 class="text-xs font-semibold text-accent-purple uppercase tracking-widest mb-2">Norminette</h4>
											<div class="space-y-2 max-h-48 overflow-y-auto">
												{#each result.norminette_check.errors as err}
													<div class="flex items-start gap-2 text-xs font-mono">
														<span class="text-accent-purple font-medium shrink-0">{err.file}:{err.line}:{err.column}:</span>
														<span class="text-ink-faint">{err.message}</span>
													</div>
												{/each}
											</div>
										</div>
									{/if}

									<!-- Compilation -->
									<div class="glass rounded-xl p-3">
										<h4 class="text-xs font-semibold text-ink-faint uppercase tracking-widest mb-2">Compilation</h4>
										<code class="block break-all font-mono text-xs text-ink-faint mb-2">$ {result.compilation.command}</code>
										{#if result.compilation.output}
											<pre class="whitespace-pre-wrap font-mono text-xs {result.compilation.success ? 'text-accent-emerald' : 'text-accent-rose'}">{result.compilation.output}</pre>
										{:else}
											<p class="text-xs font-semibold text-accent-emerald">✓ Compiled successfully</p>
										{/if}
									</div>

									<!-- Test Cases -->
									{#if result.tests && result.tests.length > 0}
										<div class="glass rounded-xl p-3">
											<h4 class="text-xs font-semibold text-ink-faint uppercase tracking-widest mb-2">Test Cases</h4>
											<div class="max-h-64 space-y-2 overflow-y-auto pr-1">
												{#each result.tests as test}
													<div class="glass rounded-lg px-3 py-2 text-xs">
														<div class="flex items-center gap-2">
															<span class="font-bold {test.passed ? 'text-accent-emerald' : 'text-accent-rose'}">{test.passed ? "✓" : "✗"}</span>
															<span class="font-mono font-semibold text-ink">{test.name}</span>
															{#if test.info}
																<span class="ml-auto text-[11px] text-ink-faint">{test.info}</span>
															{/if}
														</div>
														{#if !test.passed}
															<div class="mt-2 flex flex-wrap items-center gap-1.5">
																<span class="text-ink-faint">expected</span>
																<span class="rounded bg-accent-emerald-soft px-1.5 py-0.5 font-mono text-accent-emerald text-[10px]">{test.expected}</span>
																<span class="text-ink-faint">got</span>
																<span class="rounded bg-accent-rose-soft px-1.5 py-0.5 font-mono text-accent-rose text-[10px]">{test.got}</span>
															</div>
														{/if}
														{#if test.source}
															<pre class="mt-2 whitespace-pre-wrap rounded bg-surface-sunken p-2 font-mono text-[11px] text-ink-muted">{test.source}</pre>
														{/if}
													</div>
												{/each}
											</div>
										</div>
									{/if}

									<!-- Memory Check -->
									{#if result.memory_check}
										<div class="glass rounded-xl p-3">
											<h4 class="text-xs font-semibold text-ink-faint uppercase tracking-widest mb-2">Memory Check</h4>
											{#if result.memory_check.has_leaks}
												<p class="text-sm font-medium text-accent-amber">⚠ {result.memory_check.leak_summary}</p>
											{:else if memoryHasNoLeaks(result)}
												<p class="text-sm font-medium text-accent-emerald">✓ {result.memory_check.leak_summary || "No leaks detected"}</p>
											{:else if (result.memory_check.error_count ?? 0) > 0}
												<p class="text-sm font-medium text-accent-amber">⚠ {result.memory_check.leak_summary || "Valgrind could not run in this environment"}</p>
											{:else}
												<p class="text-sm font-medium text-accent-emerald">✓ {result.memory_check.leak_summary || "No leaks detected"}</p>
											{/if}
										</div>
									{/if}

									<!-- Sanitizer -->
									{#if result.sanitizer_check?.has_errors}
										<div class="glass rounded-xl p-3 border-accent-rose/30">
											<h4 class="text-xs font-semibold text-accent-rose uppercase tracking-widest mb-2">Sanitizer</h4>
											{#each result.sanitizer_check.errors as err}
												<p class="font-mono text-xs text-accent-rose">{err}</p>
											{/each}
										</div>
									{/if}

									<!-- Edge Case Protection -->
									{#if result.protection && Object.keys(result.protection).length > 0}
										<div class="glass rounded-xl p-3">
											<h4 class="text-xs font-semibold text-ink-faint uppercase tracking-widest mb-2">Edge Case Protection</h4>
											<div class="flex flex-wrap gap-2">
												{#each Object.entries(result.protection) as [key, value]}
													{#if value !== undefined && value !== null}
														<span class="glass-button rounded-full px-2.5 py-1 text-[11px] font-medium {value ? 'bg-accent-emerald-soft text-accent-emerald' : 'bg-accent-rose-soft text-accent-rose'}">
															{key.replaceAll("_", " ")}: {value ? "✓" : "✗"}
														</span>
													{/if}
												{/each}
											</div>
										</div>
									{/if}

									<!-- Hints -->
									{#if result.hints && result.hints.length > 0}
										<div class="glass rounded-xl p-3 border-accent-blue/30">
											<h4 class="text-xs font-semibold text-accent-blue uppercase tracking-widest mb-2">Hints</h4>
											<ul class="space-y-1.5">
												{#each result.hints as hint}
													<li class="flex items-start gap-2 text-sm text-ink">
														<span class="mt-0.5 text-accent-blue">→</span>
														{hint}
													</li>
												{/each}
											</ul>
										</div>
									{/if}
								</div>
							{/if}
						</article>
					{/each}
				</div>
			</section>
		{/if}
	{/each}
</div>
