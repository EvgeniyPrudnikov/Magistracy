
#lab 2 Prudnikov

function cdd {

    if [ $# -gt 1 ]; then 
        echo "Wrong number of arguments!"
        return 1
    fi

    subdir="${1:-}"

    if ! cd "$HOME"/Downloads/"$subdir" 2>/dev/null; then 
        echo "No such directory." 
        return 1
    fi
}

function mvd {
    
    if [ $# -gt 2 ]; then 
        echo "Wrong number of arguments!"
        return 1
    fi  

    if [ -z "$1" ]; then 
        echo "Please specify source."
        return 1
    fi

    subdir="${2:-}"

    mkdir "$HOME"/Downloads/"$subdir" 2>/dev/null

    if ! mv "$1" "$HOME"/Downloads/"$subdir"; then
        return 1    
    fi

}

function cpd {

    if [ $# -gt 2 ]; then 
        echo "Wrong number of arguments!"
        return 1
    fi  

    if [ -z "$1" ]; then 
        echo "Please specify source."
        return 1
    fi

    subdir="${2:-}"

    mkdir "$HOME"/Downloads/"$subdir" 2>/dev/null

    if ! cp -r "$1" "$HOME"/Downloads/"$subdir"; then
        return 1
    fi

}

function mvfd {
    
    if [ $# -gt 2 ]; then 
        echo "Wrong number of arguments!"
        return 1
    fi  

    if [ -z "$1" ]; then 
        echo "please specify source"
        return 1
    fi
    
    sourceSubDir="$1"
    targetSubDir="${2:-.}"

    mkdir "$targetSubDir" 2>/dev/null

    if ! mv "$HOME"/Downloads/"$sourceSubDir" "$targetSubDir"; then
        return 1
    fi

}

function cpfd {
    
    if [ $# -gt 2 ]; then 
        echo "Wrong number of arguments!"
        return 1
    fi  

    if [ -z "$1" ]; then 
        echo "Please specify source."
        return 1
    fi

    sourceSubDir="$1"
    targetSubDir="${2:-.}"

    mkdir "$targetSubDir" 2>/dev/null

    if ! cp -r "$HOME"/Downloads/"$sourceSubDir" "$targetSubDir"; then
        return 1
    fi

}