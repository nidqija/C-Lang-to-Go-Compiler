




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

            // init check for default placeholder files
            const isDefaultPlaceholder = fileStore.files.length === 1 && 
            fileStore.files[0].content.includes("// Select 'Import Files'");


            // if default placeholder file in web exists
            if (isDefaultPlaceholder){
                // replace the default placeholder files with imported files
                fileStore.files = imported;

                // make the file on the first position
                fileStore.activeFileIndex = 0;
            } else {

                // iterate through all imported files , merge each imported files
                for (const newFile of imported){
                    const existingIndex = fileStore.files.findIndex(f=>f.name === newFile.name);

                // if file already open , update its content
                if(existingIndex !== -1){
                    fileStore.files[existingIndex].content = newFile.content;
                    fileStore.activeFileIndex = existingIndex;
                } else {

                    // if its a new file , append to tabs and switch focus to it
                    fileStore.files.push(newFile);
                    fileStore.activeFileIndex = fileStore.files.length -1 ;
                }
                }
            }

            input.value = "";
    }

    // function init to create new file in web ui wrapper
    export function addNewFile(){
        const filename = prompt("Enter file name (e.g., math.c, helper.h):", `file${fileStore.files.length + 1}.c`);

        if(!filename || !filename.trim() ){
            return;
        }

        fileStore.files.push({
            name : filename.trim(),
            content:  `// ${filename.trim()}\n`
        })

        fileStore.activeFileIndex = fileStore.files.length - 1;
    }

    // function init to close file in web ui wrapper
    // we will detect the files position and mouse event which is mouse clicking
    export function closeFile(indexToClose: number , event : MouseEvent){
        event.stopPropagation();

        if(fileStore.files.length <= 1){
            alert("You must have at least one file open!");
            return;
        }

        fileStore.files = fileStore.files.filter((_, idx) => idx !== indexToClose);

        if(fileStore.activeFileIndex >= fileStore.files.length){
            fileStore.activeFileIndex = fileStore.files.length - 1;
        }
    }


