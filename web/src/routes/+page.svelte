<script lang="ts">
    let cCode = $state(`int main() {
    printf("Hello from C2Go!\\n");

    for (int i = 0; i < 4; i++) {
        printf("Hello world\\n");
    }

    return 0;
}`);

    let ast = $state("");
    let goCode = $state("");
    let output = $state("");
    let exitCode = $state<number | null>(null);
    let loading = $state(false);
    let activeTab = $state<"output" | "ast" | "go">("output");

    function loadPreset(type: string) {
        if (type == "loop") {
            cCode = `int main() {\n    for (int i = 0; i < 4; i++) {\n        printf("Iteration %d\\n", i);\n    }\n    return 0;\n}`;
        } else if (type == "math") {
            cCode = `int main() {\n    return 50 - 10 / 2 + 3 * 4;\n}`;
        } else if (type == "hello") {
            cCode = `int main() {\n    printf("Hello World!\\n");\n    return 0;\n}`;
        }
    }

    async function runCompiler() {
        loading = true;
        output = "Transpiling and executing the code...";

        try {
            const res = await fetch("/api/run", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({ code: cCode })
            });

            const data = await res.json();

            ast = data.ast || "";
            goCode = data.go_code || "";
            output = data.output || (data.error ? "Error: " + data.error : "");
            exitCode = data.exit_code ?? null;
            activeTab = "output";
        } catch (err: any) {
            output = "Network Error: " + err.message;
        } finally {
            loading = false;
        }
    }
</script>

<div class="flex min-h-screen md:h-screen flex-col bg-[#1e1e1e] text-gray-200 font-mono">
	<!-- Navbar (Wraps neatly on mobile, row on desktop) -->
	<header class="flex flex-wrap items-center justify-between gap-2 border-b border-[#333] bg-[#252526] px-4 py-2.5">
		<div class="flex items-center gap-2">
			<span class="text-sm md:text-base font-bold tracking-wider text-white">⚡ C2Go Playground</span>
		</div>
		
		<!-- Presets & Run Action -->
		<div class="flex items-center gap-1.5 overflow-x-auto py-1">
			<button onclick={() => loadPreset("loop")} class="whitespace-nowrap rounded bg-[#333] px-2.5 py-1 text-xs hover:bg-[#444]">
				For Loop
			</button>
			<button onclick={() => loadPreset("math")} class="whitespace-nowrap rounded bg-[#333] px-2.5 py-1 text-xs hover:bg-[#444]">
				PEMDAS Math
			</button>
			<button onclick={() => loadPreset("hello")} class="whitespace-nowrap rounded bg-[#333] px-2.5 py-1 text-xs hover:bg-[#444]">
				Hello World
			</button>
			<button
				onclick={runCompiler}
				disabled={loading}
				class="ml-1 flex items-center gap-1 rounded bg-emerald-600 px-3.5 py-1.5 text-xs font-semibold text-white shadow transition hover:bg-emerald-500 disabled:opacity-50 active:scale-95 cursor-pointer"
			>
				{#if loading}
					<span>Running...</span>
				{:else}
					<span>▶ Run</span>
				{/if}
			</button>
		</div>
	</header>

	<!-- Main Workspace: Stacks vertically on mobile (flex-col), 50/50 split on desktop (md:flex-row) -->
	<div class="flex flex-1 flex-col overflow-y-auto md:flex-row md:overflow-hidden">
		<!-- Left / Top: C Source Code Editor -->
		<div class="flex h-[42vh] w-full flex-col border-b border-[#333] md:h-full md:w-1/2 md:border-b-0 md:border-r">
			<div class="flex items-center justify-between border-b border-[#333] bg-[#2d2d2d] px-4 py-1.5 text-xs text-gray-400">
				<span>C Source (Input)</span>
				<span class="text-[10px] text-gray-500">Editable</span>
			</div>
			<textarea
				bind:value={cCode}
				class="flex-1 resize-none bg-[#1e1e1e] p-3 font-mono text-base md:text-sm leading-relaxed text-gray-100 outline-none"
				spellcheck="false"
				placeholder="Write your C code here..."
			></textarea>
		</div>

		<!-- Right / Bottom: Output Tabs -->
		<div class="flex min-h-[45vh] w-full flex-1 flex-col bg-[#1e1e1e] md:min-h-0 md:w-1/2">
			<!-- Scrollable Tab Switcher on mobile -->
			<div class="flex items-center overflow-x-auto border-b border-[#333] bg-[#252526]">
				<button
					onclick={() => (activeTab = "output")}
					class="whitespace-nowrap px-3.5 py-2 text-xs font-medium transition {activeTab === 'output' ? 'border-b-2 border-emerald-500 bg-[#1e1e1e] text-white' : 'text-gray-400 hover:text-gray-200'}"
				>
					Console Output
				</button>
				<button
					onclick={() => (activeTab = "go")}
					class="whitespace-nowrap px-3.5 py-2 text-xs font-medium transition {activeTab === 'go' ? 'border-b-2 border-emerald-500 bg-[#1e1e1e] text-white' : 'text-gray-400 hover:text-gray-200'}"
				>
					Generated Go
				</button>
				<button
					onclick={() => (activeTab = "ast")}
					class="whitespace-nowrap px-3.5 py-2 text-xs font-medium transition {activeTab === 'ast' ? 'border-b-2 border-emerald-500 bg-[#1e1e1e] text-white' : 'text-gray-400 hover:text-gray-200'}"
				>
					AST Tree
				</button>

				{#if exitCode !== null}
					<span class="ml-auto mr-3 whitespace-nowrap rounded px-2 py-0.5 text-[11px] font-semibold {exitCode === 0 ? 'bg-emerald-900/60 text-emerald-300' : 'bg-amber-900/60 text-amber-300'}">
						Exit: {exitCode}
					</span>
				{/if}
			</div>

			<!-- Tab Content Viewers with responsive scrolling -->
			<div class="flex-1 overflow-auto p-4 font-mono text-xs sm:text-sm leading-relaxed">
				{#if activeTab === "output"}
					<pre class="whitespace-pre-wrap break-words text-emerald-400">{output || "Tap 'Run' to see program output."}</pre>
				{:else if activeTab === "go"}
					<pre class="whitespace-pre-wrap break-words text-blue-300">{goCode || "// Generated Go code will appear here"}</pre>
				{:else if activeTab === "ast"}
					<pre class="whitespace-pre-wrap break-words text-amber-200">{ast || "// AST Tree will appear here"}</pre>
				{/if}
			</div>
		</div>
	</div>
</div>
