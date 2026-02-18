<script lang="ts">
	import type { FunctionInfo, NorminetteResult } from "$lib/api";
	import {
		PART_ORDER,
		getFunctionPart,
		type LibftPart,
	} from "$lib/libftParts";

	let {
		functions,
		norminette,
		selected = $bindable([]),
		onRun,
		onCancel,
	}: {
		functions: FunctionInfo[];
		norminette: NorminetteResult | null;
		selected: string[];
		onRun: () => void;
		onCancel: () => void;
	} = $props();

	function toggle(name: string) {
		if (selected.includes(name)) {
			selected = selected.filter((n) => n !== name);
		} else {
			selected = [...selected, name];
		}
	}

	function isAllSelected(): boolean {
		return functions.length > 0 && selected.length === functions.length;
	}

	function toggleAll() {
		selected = isAllSelected() ? [] : functions.map((f) => f.name);
	}

	function getFunctionsByPart(part: LibftPart): FunctionInfo[] {
		return functions.filter((func) => getFunctionPart(func.name) === part);
	}

	function selectedInPart(part: LibftPart): number {
		return getFunctionsByPart(part).filter((func) =>
			selected.includes(func.name),
		).length;
	}

	function isPartFullySelected(part: LibftPart): boolean {
		const partFunctions = getFunctionsByPart(part);
		return (
			partFunctions.length > 0 &&
			partFunctions.every((func) => selected.includes(func.name))
		);
	}

	function togglePart(part: LibftPart) {
		const partNames = getFunctionsByPart(part).map((func) => func.name);
		if (partNames.length === 0) return;

		if (isPartFullySelected(part)) {
			selected = selected.filter((name) => !partNames.includes(name));
			return;
		}

		selected = Array.from(new Set([...selected, ...partNames]));
	}

	const partColors: Record<LibftPart, { bg: string; text: string; dot: string }> = {
		"Part 1": { bg: "bg-accent-blue-soft", text: "text-accent-blue", dot: "bg-accent-blue" },
		"Part 2": { bg: "bg-accent-purple-soft", text: "text-accent-purple", dot: "bg-accent-purple" },
		"Part 3": { bg: "bg-accent-emerald-soft", text: "text-accent-emerald", dot: "bg-accent-emerald" },
	};
</script>

<div class="glass rounded-3xl p-6">
	<!-- Header -->
	<div class="flex flex-col sm:flex-row sm:items-center justify-between gap-4 mb-8 pb-6 border-b border-border-soft">
		<div>
			<h2 class="text-xl font-semibold text-ink">
				{functions.length} functions detected
			</h2>
			<div class="flex items-center gap-4 mt-1">
				<p class="text-ink-faint text-sm">
					<span class="font-mono text-accent-blue font-medium">{selected.length}</span> selected
				</p>
				{#if norminette}
					{#if norminette.has_errors}
						<span class="inline-flex items-center gap-1.5 rounded-full px-2.5 py-1 text-xs font-medium bg-accent-rose-soft text-accent-rose">
							<svg class="w-3.5 h-3.5" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
								<path stroke-linecap="round" stroke-linejoin="round" d="M12 9v3.75m-9.303 3.376c-.866 1.5.217 3.374 1.948 3.374h14.71c1.73 0 2.813-1.874 1.948-3.374L13.949 3.378c-.866-1.5-3.032-1.5-3.898 0L2.697 16.126zM12 15.75h.007v.008H12v-.008z" />
							</svg>
							{norminette.total_errors} norminette {norminette.total_errors === 1 ? 'error' : 'errors'}
						</span>
					{:else}
						<span class="inline-flex items-center gap-1.5 rounded-full px-2.5 py-1 text-xs font-medium bg-accent-emerald-soft text-accent-emerald">
							<svg class="w-3.5 h-3.5" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
								<path stroke-linecap="round" stroke-linejoin="round" d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
							</svg>
							Norminette OK
						</span>
					{/if}
				{/if}
			</div>
		</div>
		<div class="flex items-center gap-3">
			<button
				class="glass-button rounded-full px-4 py-2 text-sm font-medium text-ink-muted hover:text-ink"
				onclick={toggleAll}
			>
				{isAllSelected() ? "Deselect all" : "Select all"}
			</button>
			<button
				class="glass-button rounded-full px-4 py-2 text-sm font-medium text-ink-muted hover:text-accent-rose"
				onclick={onCancel}
			>
				Cancel
			</button>
			<button
				class="rounded-full px-5 py-2 text-sm font-medium text-white bg-accent-blue hover:bg-accent-blue/90 transition-all disabled:opacity-40 disabled:cursor-not-allowed disabled:hover:bg-accent-blue"
				disabled={selected.length === 0}
				onclick={onRun}
			>
				Run tests
			</button>
		</div>
	</div>

	<!-- Function sections -->
	<div class="space-y-8">
		{#each PART_ORDER as part}
			{@const partFunctions = getFunctionsByPart(part)}
			{@const colors = partColors[part]}
			{#if partFunctions.length > 0}
				<section class="animate-slide-up" style="animation-delay: {PART_ORDER.indexOf(part) * 0.1}s">
					<div class="flex items-center gap-3 mb-4">
						<div class="w-2 h-2 rounded-full {colors.dot}"></div>
						<h3 class="text-xs font-semibold text-ink-faint uppercase tracking-widest">{part}</h3>
						<div class="flex-1 h-px bg-border-soft"></div>
						<span class="glass-button rounded-full px-2.5 py-0.5 text-[11px] font-medium text-ink-faint">
							{selectedInPart(part)}/{partFunctions.length}
						</span>
						<button
							class="glass-button rounded-full px-2.5 py-0.5 text-[11px] font-medium text-ink-muted hover:text-ink"
							onclick={() => togglePart(part)}
						>
							{isPartFullySelected(part) ? "Deselect" : "Select"}
						</button>
					</div>
					<div class="grid grid-cols-3 sm:grid-cols-4 md:grid-cols-5 lg:grid-cols-6 gap-2">
						{#each partFunctions as func}
							<button
								class="group relative rounded-xl px-3 py-2.5 text-sm font-mono font-medium transition-all duration-200 min-h-[42px] flex items-center justify-center {selected.includes(func.name)
									? `${colors.bg} ${colors.text} border border-border-soft`
									: 'text-ink-faint border border-transparent hover:border-border-soft hover:text-ink'}"
								onclick={() => toggle(func.name)}
								title={func.name}
							>
								<span class="relative z-10 truncate w-full text-center">{func.name.replace("ft_", "")}</span>
								{#if selected.includes(func.name)}
									<div class="absolute inset-0 rounded-xl bg-gradient-to-br from-white/20 to-transparent opacity-0 group-hover:opacity-100 transition-opacity"></div>
								{/if}
							</button>
						{/each}
					</div>
				</section>
			{/if}
		{/each}
	</div>
</div>
