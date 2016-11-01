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