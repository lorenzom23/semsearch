#!/bin/bash
set -euo pipefail

MODEL_DIR="${XDG_DATA_HOME:-$HOME/.local/share}/semsearch/model"
BASE_URL="https://huggingface.co/sentence-transformers/paraphrase-multilingual-MiniLM-L12-v2/resolve/main"

mkdir -p "$MODEL_DIR"

# check if the file alreasy exists
if [ -f "$MODEL_DIR/model.onnx" ]; then
    echo "model.onnx already present, skipping"
else
    wget -q --show-progress -O "$MODEL_DIR/model.onnx.part" "$BASE_URL/onnx/model.onnx"
    mv "$MODEL_DIR/model.onnx.part" "$MODEL_DIR/model.onnx"
    echo "model.onnx downloaded!"
fi

# check if the file alreasy exists
if [ -f "$MODEL_DIR/sentencepiece.bpe.model" ]; then
    echo "sentencepiece.bpe.model already present, skipping"
else
    wget -q --show-progress -O "$MODEL_DIR/sentencepiece.bpe.model.part" "$BASE_URL/sentencepiece.bpe.model"
    mv "$MODEL_DIR/sentencepiece.bpe.model.part" "$MODEL_DIR/sentencepiece.bpe.model"
    echo "sentencepiece.bpe.model downloaded!"
fi

