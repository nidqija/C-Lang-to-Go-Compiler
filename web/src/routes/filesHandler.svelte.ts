




    type SourceFiles = {
        name : string,
        content : string;
    }


    export const fileStore = $state({
         files :[
        {
        name: "main.c",
        content: `// Select 'Import Files' or type here\nint main() {\n    return 0;\n}`
      }
    ] as SourceFiles[],
    activeFileIndex :0
    });
   


    let activeFileIndex = $state(0);


    export async function handleImportFiles(event: Event){
            const input = event.target as HTMLInputElement;

            if(!input.files || input.files.length === 0) {
                return;
            }


            const imported : SourceFiles[] = [];


            for ( const file of Array.from(input.files)){
                const text = await file.text();

                imported.push({
                    name : file.name,
                    content : text
                });
            }

            fileStore.files = imported;
            fileStore.activeFileIndex = 0;
    }


