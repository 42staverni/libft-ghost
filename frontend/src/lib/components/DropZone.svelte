<script lang="ts">
	import JSZip from "jszip";

	let {
		onFileSelect,
		uploading = false,
	}: {
		onFileSelect: (file: File) => void;
		uploading?: boolean;
	} = $props();

	let dragging = $state(false);
	let fileInput: HTMLInputElement | undefined = $state(undefined);

	async function folderToZip(entries: FileSystemEntry[]): Promise<File> {
		const zip = new JSZip();

		async function addEntries(items: FileSystemEntry[], folder: any) {
			for (const entry of items) {
				if (entry.isFile) {
					const file = await new Promise<File>((resolve) => {
						(entry as FileSystemFileEntry).file(resolve);
					});
					folder.file(file.name, file);
				} else if (entry.isDirectory) {
					const reader = (entry as FileSystemDirectoryEntry).createReader();
					const children = await new Promise<FileSystemEntry[]>((resolve) => {
						reader.readEntries(resolve);
					});
					const subFolder = folder.folder((entry as FileSystemDirectoryEntry).name);
					await addEntries(children, subFolder);
				}
			}
		}

		await addEntries(entries, zip);
		return new File(
			[await zip.generateAsync({ type: "blob" })],
			"libft.zip",
			{
				type: "application/zip",
			},
		);
	}

	async function handleDrop(e: DragEvent) {
		e.preventDefault();
		dragging = false;
		const items = e.dataTransfer?.items;
		if (!items) return;

		const folderEntries: FileSystemEntry[] = [];
		for (let i = 0; i < items.length; i++) {
			const item = items[i];
			if (item.kind === "file") {
				const file = item.getAsFile();
				if (file && file.name.endsWith(".zip")) {
					onFileSelect(file);
					return;
				}
			}
			const entry = items[i].webkitGetAsEntry?.();
			if (entry?.isDirectory) {
				folderEntries.push(entry);
			}
		}

		if (folderEntries.length > 0) {
			try {
				const zip = await folderToZip(folderEntries);
				onFileSelect(zip);
				return;
			} catch (err) {
				console.error("Failed to process folder:", err);
			}
		}

		const file = items[0]?.getAsFile();
		if (file) onFileSelect(file);
	}

	function handleFileInput(e: Event) {
		const input = e.target as HTMLInputElement;
		if (input.files?.[0]) {
			onFileSelect(input.files[0]);
		}
	}
</script>

{#if uploading}
	<div class="glass rounded-3xl p-12 text-center">
		<div class="relative mb-6">
			<div class="w-16 h-16 mx-auto rounded-2xl bg-accent-blue-soft flex items-center justify-center animate-pulse">
				<svg class="w-8 h-8 text-accent-blue" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
					<path stroke-linecap="round" stroke-linejoin="round" d="M3 16.5v2.25A2.25 2.25 0 005.25 21h13.5A2.25 2.25 0 0021 18.75V16.5m-13.5-9L12 3m0 0l4.5 4.5M12 3v13.5" />
				</svg>
			</div>
			<div class="absolute inset-0 flex items-center justify-center">
				<div class="w-24 h-24 rounded-2xl border-2 border-accent-blue animate-ping opacity-20"></div>
			</div>
		</div>
		<p class="text-ink font-medium">Analyzing your project...</p>
		<p class="text-ink-faint text-sm mt-1">This will just take a moment</p>
	</div>
{:else}
	<div
		class="relative glass-strong rounded-3xl transition-all duration-300 cursor-pointer overflow-hidden group {dragging ? 'scale-[1.02] shadow-2xl' : 'hover:shadow-xl'}"
		role="button"
		tabindex="0"
		aria-label="Drop zone for libft project"
		ondragover={(e) => {
			e.preventDefault();
			dragging = true;
		}}
		ondragleave={() => (dragging = false)}
		ondrop={handleDrop}
		onclick={() => fileInput?.click()}
		onkeydown={(e) => {
			if (e.key === "Enter" || e.key === " ") {
				e.preventDefault();
				fileInput?.click();
			}
		}}
	>
		<!-- Animated gradient border -->
		<div class="absolute inset-0 rounded-3xl transition-opacity duration-300 {dragging ? 'opacity-100' : 'opacity-0'}">
			<div class="absolute inset-0 rounded-3xl bg-gradient-to-r from-accent-blue via-accent-purple to-accent-emerald p-[2px]">
				<div class="w-full h-full rounded-3xl bg-surface"></div>
			</div>
		</div>

		<!-- Default border -->
		<div class="absolute inset-0 rounded-3xl border-2 border-dashed border-border-soft transition-all duration-300 {dragging ? 'border-transparent' : 'group-hover:border-border-soft-hover'}"></div>

		<!-- Content -->
		<div class="relative flex flex-col items-center justify-center py-20 px-8 text-center">
			<!-- Upload icon -->
			<div class="relative mb-8">
				<div class="absolute inset-0 rounded-3xl bg-gradient-to-br from-accent-blue-soft via-accent-purple-soft to-accent-emerald-soft blur-2xl transition-transform duration-500 {dragging ? 'scale-150' : 'group-hover:scale-125'}"></div>
				<div class="relative w-20 h-20 rounded-2xl bg-gradient-to-br from-accent-blue/20 to-accent-purple/20 flex items-center justify-center border border-border-soft transition-transform duration-300 {dragging ? 'scale-110 rotate-3' : 'group-hover:scale-105'}">
					<svg
						class="w-10 h-10 text-accent-blue transition-all duration-300 {dragging ? 'scale-110' : ''}"
						fill="none"
						viewBox="0 0 24 24"
						stroke="currentColor"
						stroke-width="1.5"
					>
						<path
							stroke-linecap="round"
							stroke-linejoin="round"
							d="M3 16.5v2.25A2.25 2.25 0 005.25 21h13.5A2.25 2.25 0 0021 18.75V16.5m-13.5-9L12 3m0 0l4.5 4.5M12 3v13.5"
						/>
					</svg>
				</div>
			</div>

			<h2 class="text-2xl font-semibold text-ink mb-3">
				{dragging ? "Drop it here" : "Drop your Libft project"}
			</h2>
			<p class="text-ink-muted">
				Drag & drop a <span class="font-mono text-sm bg-accent-emerald-soft px-1.5 py-0.5 rounded">folder</span>
				or <span class="font-mono text-sm bg-accent-amber-soft px-1.5 py-0.5 rounded">.zip</span> file
			</p>
			<p class="text-ink-faint text-sm mt-3">
				or click to browse
			</p>
		</div>

		<input
			bind:this={fileInput}
			type="file"
			accept=".zip"
			class="hidden"
			onchange={handleFileInput}
		/>
	</div>
	
	<!-- Project Structure Guide -->
	<div class="mt-6 glass rounded-2xl p-5">
		<div class="flex items-center gap-2 mb-3">
			<svg class="w-4 h-4 text-accent-blue" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
				<path stroke-linecap="round" stroke-linejoin="round" d="M9.879 7.519c1.171-1.025 3.071-1.025 4.242 0 1.172 1.025 1.172 2.687 0 3.712-.203.179-.43.326-.67.442-.745.361-1.45.999-1.45 1.827v.75M21 12a9 9 0 11-18 0 9 9 0 0118 0zm-9 5.25h.008v.008H12v-.008z" />
			</svg>
			<h3 class="text-sm font-semibold text-ink">Project Structure Guide</h3>
		</div>
		<p class="text-xs text-ink-faint mb-3">Your project should follow this structure for best results:</p>
		<div class="font-mono text-xs text-ink-muted bg-surface-sunken rounded-lg p-3 space-y-1">
			<div class="flex items-center gap-2">
				<svg class="w-4 h-4 text-accent-emerald" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
					<path stroke-linecap="round" stroke-linejoin="round" d="M2.25 12.75V12A2.25 2.25 0 014.5 9.75h15A2.25 2.25 0 0121.75 12v.75m-8.69-6.44l-2.12-2.12a1.5 1.5 0 00-1.061-.44H4.5A2.25 2.25 0 002.25 6v12a2.25 2.25 0 002.25 2.25h15A2.25 2.25 0 0021.75 18V9a2.25 2.25 0 00-2.25-2.25h-5.379a1.5 1.5 0 01-1.06-.44z" />
				</svg>
				<span class="text-ink">libft/</span>
			</div>
			<div class="flex items-center gap-2 pl-4">
				<svg class="w-4 h-4 text-accent-blue" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
					<path stroke-linecap="round" stroke-linejoin="round" d="M14.25 9.75L16.5 12l-2.25 2.25m-4.5 0L7.5 12l2.25-2.25M6 20.25h12A2.25 2.25 0 0020.25 18V6A2.25 2.25 0 0018 3.75H6A2.25 2.25 0 003.75 6v12A2.25 2.25 0 006 20.25z" />
					<text x="12" y="14" text-anchor="middle" font-size="7" font-weight="bold" fill="currentColor" stroke="none">mk</text>
				</svg>
				<span class="text-ink">Makefile</span>
			</div>
			<div class="flex items-center gap-2 pl-4">
				<svg class="w-4 h-4 text-accent-purple" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
					<path stroke-linecap="round" stroke-linejoin="round" d="M19.5 14.25v-2.625a3.375 3.375 0 00-3.375-3.375h-1.5A1.125 1.125 0 0113.5 7.125v-1.5a3.375 3.375 0 00-3.375-3.375H8.25m2.25 0H5.625c-.621 0-1.125.504-1.125 1.125v17.25c0 .621.504 1.125 1.125 1.125h12.75c.621 0 1.125-.504 1.125-1.125V11.25a9 9 0 00-9-9z" />
					<text x="12" y="15" text-anchor="middle" font-size="8" font-weight="bold" fill="currentColor" stroke="none">.h</text>
				</svg>
				<span class="text-ink">libft.h</span>
			</div>
			<div class="flex items-center gap-2 pl-4">
				<svg class="w-4 h-4 text-accent-amber" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
					<path stroke-linecap="round" stroke-linejoin="round" d="M17.25 6.75L22.5 12l-5.25 5.25m-10.5 0L1.5 12l5.25-5.25m7.5-3l-4.5 16.5" />
					<text x="12" y="14" text-anchor="middle" font-size="7" font-weight="bold" fill="currentColor" stroke="none">C</text>
				</svg>
				<span class="text-ink">ft_*.c</span>
				<span class="text-ink-faint">— all source files at root</span>
			</div>
			<div class="flex items-center gap-2 pl-4">
				<svg class="w-4 h-4 text-accent-rose" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
					<path stroke-linecap="round" stroke-linejoin="round" d="M20.25 6.375c0 2.278-3.694 4.125-8.25 4.125S3.75 8.653 3.75 6.375m16.5 0c0-2.278-3.694-4.125-8.25-4.125S3.75 4.097 3.75 6.375m16.5 0v11.25c0 2.278-3.694 4.125-8.25 4.125s-8.25-1.847-8.25-4.125V6.375m16.5 0v3.75m-16.5-3.75v3.75m16.5 0v3.75C20.25 16.153 16.556 18 12 18s-8.25-1.847-8.25-4.125V9.75m16.5 0c0 2.278-3.694 4.125-8.25 4.125s-8.25-1.847-8.25-4.125" />
				</svg>
				<span class="text-ink">libft.a</span>
				<span class="text-ink-faint">— produced by make</span>
			</div>
		</div>
		<div class="mt-3 space-y-1.5">
			<p class="text-xs text-ink-faint flex items-center gap-1.5">
				<span class="w-1 h-1 rounded-full bg-accent-emerald"></span>
				<span>Makefile must have: <span class="font-mono bg-accent-emerald-soft/30 px-1 rounded">all</span>, <span class="font-mono bg-accent-emerald-soft/30 px-1 rounded">clean</span>, <span class="font-mono bg-accent-emerald-soft/30 px-1 rounded">fclean</span>, <span class="font-mono bg-accent-emerald-soft/30 px-1 rounded">re</span>, <span class="font-mono bg-accent-emerald-soft/30 px-1 rounded">bonus</span></span>
			</p>
			<p class="text-xs text-ink-faint flex items-center gap-1.5">
				<span class="w-1 h-1 rounded-full bg-accent-blue"></span>
				<span>All <span class="font-mono">.c</span> files should be at the root level</span>
			</p>
			<p class="text-xs text-ink-faint flex items-center gap-1.5">
				<span class="w-1 h-1 rounded-full bg-accent-purple"></span>
				<span>Bonus files can be <span class="font-mono">ft_*_bonus.c</span> or just <span class="font-mono">ft_*.c</span></span>
			</p>
		</div>
	</div>
{/if}
