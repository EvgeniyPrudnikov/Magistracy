
#lab 2 Prudnikov

function cdd {

    if [ $# -gt 1 ]; then 
        echo "Wrong number of arguments!" 1>&2
        return 1
    fi

    subdir="${1:-}"

    cd "$HOME"/Downloads/"$subdir"

}

function mvd {
    
    if [ $# -gt 2 ]; then 
        echo "Wrong number of arguments!" 1>&2
        return 1
    fi  

    if [ -z "$1" ]; then 
        echo "Please specify source." 1>&2
        return 1
    fi

    subdir="${2:-}"

    impDir="$HOME"/Downloads/"$subdir"

    mkdir "$impDir" 2>/dev/null

    mv "$1" "$impDir";


}

function cpd {

    if [ $# -gt 2 ]; then 
        echo "Wrong number of arguments!" 1>&2
        return 1
    fi  

    if [ -z "$1" ]; then 
        echo "Please specify source." 1>&2
        return 1
    fi

    subdir="${2:-}"

    impDir="$HOME"/Downloads/"$subdir"

    mkdir "$impDir" 2>/dev/null

    cp -r "$1" "$impDir"

}

function mvfd {
    
    if [ $# -gt 2 ]; then 
        echo "Wrong number of arguments!" 1>&2
        return 1
    fi  

    if [ -z "$1" ]; then 
        echo "please specify source" 1>&2
        return 1
    fi
    
    sourceSubDir="$1"
    targetSubDir="${2:-.}"

    impDir="$HOME"/Downloads/"$sourceSubDir"

    mkdir "$targetSubDir" 2>/dev/null

    mv "$impDir" "$targetSubDir"

}

function cpfd {
    
    if [ $# -gt 2 ]; then 
        echo "Wrong number of arguments!" 1>&2
        return 1
    fi  

    if [ -z "$1" ]; then 
        echo "Please specify source." 1>&2
        return 1
    fi

    sourceSubDir="$1"
    targetSubDir="${2:-.}"

	impDir="$HOME"/Downloads/"$sourceSubDir"

    mkdir "$targetSubDir" 2>/dev/null

    cp -r "$impDir" "$targetSubDir"

}