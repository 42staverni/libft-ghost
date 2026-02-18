<script lang="ts">
	import "../app.css";
	import { onMount } from "svelte";
	import { theme } from "$lib/theme";
	import ThemeToggle from "$lib/components/ThemeToggle.svelte";

	let { children } = $props();

	onMount(() => {
		theme.init();
	});
</script>

<div class="min-h-screen bg-surface relative overflow-x-hidden transition-colors duration-500">
	<!-- Organic gradient orbs background -->
	<div class="fixed inset-0 pointer-events-none" aria-hidden="true">
		<!-- Large gradient blob - blue/purple -->
		<div class="absolute top-0 left-1/4 w-[600px] h-[600px] rounded-full bg-gradient-to-br from-blue-400/20 via-purple-400/15 to-transparent blur-[100px] animate-blob"></div>
		
		<!-- Large gradient blob - emerald/cyan -->
		<div class="absolute top-1/3 right-1/4 w-[500px] h-[500px] rounded-full bg-gradient-to-bl from-emerald-400/15 via-cyan-400/10 to-transparent blur-[100px] animate-blob animation-delay-2000"></div>
		
		<!-- Medium gradient blob - rose/pink -->
		<div class="absolute bottom-1/4 left-1/3 w-[400px] h-[400px] rounded-full bg-gradient-to-tr from-rose-400/15 via-pink-300/10 to-transparent blur-[100px] animate-blob animation-delay-4000"></div>
		
		<!-- Small gradient blob - amber/orange -->
		<div class="absolute top-2/3 right-1/3 w-[300px] h-[300px] rounded-full bg-gradient-to-tl from-amber-400/10 via-orange-300/8 to-transparent blur-[80px] animate-blob animation-delay-6000"></div>
		
		<!-- Subtle noise texture overlay -->
		<div class="absolute inset-0 opacity-[0.015]" style="background-image: url('data:image/svg+xml,%3Csvg viewBox=%220 0 256 256%22 xmlns=%22http://www.w3.org/2000/svg%22%3E%3Cfilter id=%22noiseFilter%22%3E%3CfeTurbulence type=%22fractalNoise%22 baseFrequency=%220.9%22 numOctaves=%224%22 stitchTiles=%22stitch%22/%3E%3C/filter%3E%3Crect width=%22100%25%22 height=%22100%25%22 filter=%22url(%23noiseFilter)%22/%3E%3C/svg%3E');"></div>
	</div>

	<!-- Fixed theme toggle -->
	<div class="fixed top-6 right-6 z-50">
		<ThemeToggle />
	</div>

	<!-- Main content -->
	<main class="relative z-10">
		{@render children()}
	</main>
</div>
