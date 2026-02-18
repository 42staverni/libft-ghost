<script lang="ts">
	import { theme } from "$lib/theme";

	function toggleTheme() {
		theme.toggle();
	}
</script>

<button
	class="glass-button group relative rounded-full p-3 transition-all duration-300 hover:scale-110"
	onclick={toggleTheme}
	aria-label="Toggle theme"
	type="button"
>
	<!-- Sun icon for light mode -->
	<svg
		class="h-5 w-5 text-ink transition-all duration-300 rotate-0 scale-100 dark:-rotate-90 dark:scale-0 dark:opacity-0"
		viewBox="0 0 24 24"
		fill="none"
		stroke="currentColor"
		stroke-width="1.5"
	>
		<circle cx="12" cy="12" r="4"></circle>
		<path d="M12 2v2"></path>
		<path d="M12 20v2"></path>
		<path d="m4.93 4.93 1.41 1.41"></path>
		<path d="m17.66 17.66 1.41 1.41"></path>
		<path d="M2 12h2"></path>
		<path d="M20 12h2"></path>
		<path d="m6.34 17.66-1.41 1.41"></path>
		<path d="m19.07 4.93-1.41 1.41"></path>
	</svg>
	
	<!-- Moon icon for dark mode -->
	<svg
		class="absolute inset-0 m-auto h-5 w-5 text-ink transition-all duration-300 -rotate-90 scale-0 opacity-0 dark:rotate-0 dark:scale-100 dark:opacity-100"
		viewBox="0 0 24 24"
		fill="none"
		stroke="currentColor"
		stroke-width="1.5"
>
		<path d="M12 3a6 6 0 0 0 9 9 9 9 0 1 1-9-9Z"></path>
	</svg>
</button>

<style>
	:global(html[data-theme="dark"]) .dark\:rotate-0 {
		--tw-rotate: 0deg;
		transform: translate(var(--tw-translate-x), var(--tw-translate-y)) rotate(var(--tw-rotate)) skewX(var(--tw-skew-x)) skewY(var(--tw-skew-y)) scaleX(var(--tw-scale-x)) scaleY(var(--tw-scale-y));
	}
	
	:global(html[data-theme="dark"]) .dark\:-rotate-90 {
		--tw-rotate: -90deg;
		transform: translate(var(--tw-translate-x), var(--tw-translate-y)) rotate(var(--tw-rotate)) skewX(var(--tw-skew-x)) skewY(var(--tw-skew-y)) scaleX(var(--tw-scale-x)) scaleY(var(--tw-scale-y));
	}
	
	:global(html[data-theme="dark"]) .dark\:scale-100 {
		--tw-scale-x: 1;
		--tw-scale-y: 1;
		transform: translate(var(--tw-translate-x), var(--tw-translate-y)) rotate(var(--tw-rotate)) skewX(var(--tw-skew-x)) skewY(var(--tw-skew-y)) scaleX(var(--tw-scale-x)) scaleY(var(--tw-scale-y));
	}
	
	:global(html[data-theme="dark"]) .dark\:scale-0 {
		--tw-scale-x: 0;
		--tw-scale-y: 0;
		transform: translate(var(--tw-translate-x), var(--tw-translate-y)) rotate(var(--tw-rotate)) skewX(var(--tw-skew-x)) skewY(var(--tw-skew-y)) scaleX(var(--tw-scale-x)) scaleY(var(--tw-scale-y));
	}
	
	:global(html[data-theme="dark"]) .dark\:opacity-100 {
		opacity: 1;
	}
	
	:global(html[data-theme="dark"]) .dark\:opacity-0 {
		opacity: 0;
	}
</style>
