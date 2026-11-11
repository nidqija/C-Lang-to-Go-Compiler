<script>
    let count = $state(0);
</script>

<div class="flex flex-col items-center justify-center h-screen">


<h1 style="font-size: 40px;">Welcome to SvelteKit</h1>

<button onclick={() => count++} class="px-4 py-2 bg-blue-600 text-white rounded">
    Clicks {count}
</button>
</div>