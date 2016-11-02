# using from .bashsrc

function cdd {

    if [ $# -gt 1 ]; then 
        echo "Wrong number of arguments!"
        return 1
    fi

    subdir="${1:-}"

    if ! cd "$HOME/Downloads/"$subdir"" 2>/dev/null; then 
        echo "No such directory" 
        return 1
    fi
}

function mvd {
    
    if [ $# -gt 2 ]; then 
        echo "wrong number of arguments"
        return 1
    fi  

    if [ -z "$1" ]; then 
        echo "please specify source"
        return 1
    fi

    subdir="${2:-}"

    echo "$subdir"

    if ! mv "$1" "$HOME/Downloads/"$subdir""; then
        echo "error"
        return 1
    fi

}