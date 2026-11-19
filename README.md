# semsearch
 
Semantic search for text files, written in C++.
 
Instead of matching keywords, `semsearch` matches meaning: files are split into chunks, each chunk is turned into an embedding with a multilingual MiniLM model (ONNX Runtime), and a query is compared against all of them. Everything runs locally.
 
## Build
 
In the root of the project execute:
 
```bash
cmake -S . -B build
cmake --build build -j
```
 
Then download the model (it will be saved in the `~/.local/share/semsearch/model/` folder), still from the root of the project:
 
```bash
./scripts/model_download.sh
```
 
## Usage
 
Before searching, the data must be indexed. Run `index` with the path of the folder to index (`.txt`, `.md` and `.rst` files, hidden files and folders are skipped):
 
```bash
./build/semsearch index <folder_path>
```
 
The index is stored in `~/.local/share/semsearch/`. Running `index` again rebuilds it from scratch.
 
Then search with `query`. The search is performed only on the data that has been indexed:
 
```bash
./build/semsearch query "<text>"
```
 
The top 3 results are printed with their score and source file.
 
### Demo data
 
The `search_demo` folder contains some example text files that share the same keyword used in different contexts, to show that the search follows the meaning and not the words.
 
```bash
./build/semsearch index search_demo
./build/semsearch query "I want to open a savings account"
```
