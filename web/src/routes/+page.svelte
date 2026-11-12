<script lang="ts">
    let cCode = $state(`int main() {
        
        printf("Hello from C2Go!\\n");

        for(int i = 0 ; i < 4 ; i++){
        printf("Hello world");
        }
    }
     return 0;  

}`);


    let ast = $state("");
    let goCode = $state("");
    let output = $state("");
    let exitCode = $state<number | null>(null);
    let loading = $state(false);
    let activeTab = $state<"output" | "ast" | "go">("output");

    function loadPreset( type:string){
        if(type == "loop"){
            cCode = `int main() {\n    for (int i = 0; i < 4; i++) {\n        printf("Iteration %d\\n", i);\n    }\n    return 0;\n}`;
        }

        else if (type == "math"){
            cCode = `int main() {\n    return 50 - 10 / 2 + 3 * 4;\n}`;
        } 

        else if (type == "hello"){
            cCode = `int main() {\n    printf("Hello World!\\n");\n    return 0;\n}`;
        }
    }

    async function runCompiler(){
        loading = true;
        output = "Transpiling and executing the code...";

        try {
            const res = await fetch("/api/run" , {
                method:"POST",
                headers :{ "Content-Type" : "application/json"},
                body : JSON.stringify({code : cCode})
            });

            const data = await res.json();

            ast = data.ast;
            goCode = data.go_code;
            output = data.output;
            exitCode = data.exit_code;
            activeTab = "output";


        } catch (err : any){
            output = "Network Error: " + err.message;
        } finally {
            loading = false;  
        }
    }

</script>


<div class="flex h-screen flex-col bg-[#1e1e1e] text-gray-200 font-mono">
	<!-- Navbar -->
	<header class="flex items-center justify-between border-b border-[#333] bg-[#252526] px-4 py-2">
		<div class="flex items-center gap-3">
			<span class="font-bold tracking-wider text-white">C2Go Playground</span>
		</div>
		<!-- Presets & Run Button -->
		<div class="flex items-center gap-2">
			<button onclick={() => loadPreset("loop")} class="rounded bg-[#333] px-3 py-1 text-xs hover:bg-[#444]">
				For Loop
			</button>
			<button onclick={() => loadPreset("math")} class="rounded bg-[#333] px-3 py-1 text-xs hover:bg-[#444]">
				PEMDAS Math
			</button>
			<button onclick={() => loadPreset("hello")} class="rounded bg-[#333] px-3 py-1 text-xs hover:bg-[#444]">
				Hello World
			</button>
			<button
				onclick={runCompiler}
				disabled={loading}
				class="ml-2 flex items-center gap-1.5 rounded bg-emerald-600 px-4 py-1.5 text-xs font-semibold text-white shadow transition hover:bg-emerald-500 disabled:opacity-50"
			>
				{#if loading}
					<span>Running...</span>
				{:else}
					<span>▶ Transpile & Run</span>
				{/if}
			</button>
		</div>
	</header>
	<!-- Main Workspace Split Pane -->
	<div class="flex flex-1 overflow-hidden">
		<!-- Left: C Source Code Editor -->
		<div class="flex w-1/2 flex-col border-r border-[#333]">
			<div class="border-b border-[#333] bg-[#2d2d2d] px-4 py-1.5 text-xs text-gray-400">
				C Source (Input)
			</div>
			<textarea
				bind:value={cCode}
				class="flex-1 resize-none bg-[#1e1e1e] p-4 font-mono text-sm leading-relaxed text-gray-100 outline-none"
				spellcheck="false"
			></textarea>
		</div>
		<!-- Right: Output Tabs -->
		<div class="flex w-1/2 flex-col bg-[#1e1e1e]">
			<!-- Tab Switcher -->
			<div class="flex items-center border-b border-[#333] bg-[#252526]">
				<button
					onclick={() => (activeTab = "output")}
					class="px-4 py-2 text-xs font-medium transition {activeTab === 'output' ? 'border-b-2 border-emerald-500 bg-[#1e1e1e] text-white' : 'text-gray-400 hover:text-gray-200'}"
				>
					Console Output
				</button>
				<button
					onclick={() => (activeTab = "go")}
					class="px-4 py-2 text-xs font-medium transition {activeTab === 'go' ? 'border-b-2 border-emerald-500 bg-[#1e1e1e] text-white' : 'text-gray-400 hover:text-gray-200'}"
				>
					Generated Go Code
				</button>
				<button
					onclick={() => (activeTab = "ast")}
					class="px-4 py-2 text-xs font-medium transition {activeTab === 'ast' ? 'border-b-2 border-emerald-500 bg-[#1e1e1e] text-white' : 'text-gray-400 hover:text-gray-200'}"
				>
					AST Tree
				</button>
				{#if exitCode !== null}
					<span class="ml-auto mr-4 rounded px-2 py-0.5 text-xs font-semibold {exitCode === 0 ? 'bg-emerald-900/60 text-emerald-300' : 'bg-amber-900/60 text-amber-300'}">
						Exit Code: {exitCode}
					</span>
				{/if}
			</div>
			<!-- Tab Content Viewers -->
			<div class="flex-1 overflow-auto p-4 font-mono text-sm leading-relaxed">
				{#if activeTab === "output"}
					<pre class="text-emerald-400">{output || "Click 'Transpile & Run' to see program output."}</pre>
				{:else if activeTab === "go"}
					<pre class="text-blue-300">{goCode || "// Generated Go code will appear here"}</pre>
				{:else if activeTab === "ast"}
					<pre class="text-amber-200">{ast || "// AST Tree will appear here"}</pre>
				{/if}
			</div>
		</div>
	</div>
</div>
