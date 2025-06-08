#!/bin/bash
fileName=$1
sourceFolder="/mnt/c/Users/fedep/Pictures/Lighshot"
destinationWSL="/home/fpuchetta/materias/Materias/algo2/tp-hash/HASH-ENUNCIADO/img/"

sourceFile="$sourceFolder/$fileName"
destinationFile="$destinationWSL/$fileName"

if [ -f "$sourceFile" ]; then
    cp "$sourceFile" "$destinationFile"
    echo "Archivo $fileName copiado a WSL."
else
    echo "El archivo $fileName no se encontró en la carpeta."
fi