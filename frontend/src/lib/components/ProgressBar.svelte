<script lang="ts">
	import type { ProgressEvent } from '$lib/api';

	let {
		progress,
	}: {
		progress: ProgressEvent | null;
	} = $props();
</script>

{#if progress}
	<div class="glass rounded-2xl p-5">
		<div class="flex items-center justify-between mb-4">
			<div class="flex items-center gap-3">
				<div class="w-2 h-2 rounded-full bg-accent-blue animate-pulse"></div>
				<span class="font-mono font-semibold text-ink">{progress.function}</span>
				<span class="text-sm text-ink-faint">{progress.step}</span>
			</div>
			<span class="text-sm font-mono text-ink-muted">{progress.current}/{progress.total}</span>
		</div>
		<div class="h-2 rounded-full bg-surface-sunken overflow-hidden">
			<div
				class="h-full rounded-full bg-gradient-to-r from-accent-blue via-accent-purple to-accent-emerald transition-all duration-500 ease-out"
				style="width: {(progress.current / progress.total) * 100}%"
			></div>
		</div>
	</div>
{/if}
