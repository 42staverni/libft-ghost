import { writable, type Writable } from 'svelte/store';

type Theme = 'light' | 'dark';

function createThemeStore(): Writable<Theme> & { toggle: () => void; init: () => void } {
	let currentTheme: Theme = 'light';

	// Determine initial theme
	if (typeof window !== 'undefined') {
		const stored = localStorage.getItem('theme') as Theme | null;
		currentTheme = stored || 'light';
		document.documentElement.setAttribute('data-theme', currentTheme);
	}

	const { subscribe, set } = writable<Theme>(currentTheme);
	const update = (updater: (value: Theme) => Theme) => {
		const next = updater(currentTheme);
		currentTheme = next;
		set(next);
		if (typeof window !== 'undefined') {
			localStorage.setItem('theme', next);
			document.documentElement.setAttribute('data-theme', next);
		}
	};

	return {
		subscribe,
		set: (value: Theme) => {
			currentTheme = value;
			set(value);
			if (typeof window !== 'undefined') {
				localStorage.setItem('theme', value);
				document.documentElement.setAttribute('data-theme', value);
			}
		},
		update,
		toggle: () => {
			const newTheme = currentTheme === 'light' ? 'dark' : 'light';
			currentTheme = newTheme;
			set(newTheme);
			if (typeof window !== 'undefined') {
				localStorage.setItem('theme', newTheme);
				document.documentElement.setAttribute('data-theme', newTheme);
			}
		},
		init: () => {
			if (typeof window !== 'undefined') {
				const stored = localStorage.getItem('theme') as Theme | null;
				const theme = stored || 'light';
				currentTheme = theme;
				document.documentElement.setAttribute('data-theme', theme);
				set(theme);
			}
		}
	};
}

export const theme = createThemeStore();


