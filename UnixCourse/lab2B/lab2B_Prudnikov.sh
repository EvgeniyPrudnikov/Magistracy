
#lab 2 Prudnikov

MAINDIR="$HOME"/Downloads/

function cdd {

    if [ $# -gt 1 ]; then 
        echo "Wrong number of arguments!" 1>&2
        return 1
    fi

    subdir="${1:-}"

    cd "$MAINDIR""$subdir"

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

    impDir="$MAINDIR""$subdir"

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

    impDir="$MAINDIR""$subdir"

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

    impDir="$MAINDIR""$sourceSubDir"

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

	impDir="$MAINDIR""$sourceSubDir"

    mkdir "$targetSubDir" 2>/dev/null

    cp -r "$impDir" "$targetSubDir"

}


_complete_file() {
    _init_completion -s || return
    _filedir
}

_compl_cd() {
    CDPATH="$MAINDIR" _cd
}

_compl_mvcp() {
    if [[ "$COMP_CWORD" == "$1" ]] ; then
        _cur_pwd="$(pwd)"
        cd "$MAINDIR" && _complete_file
        cd "$_cur_pwd"
    else
        _complete_file
    fi
}

_compl_mvh() {
    _compl_mvcp 2
}

_compl_mvfh() {
    _compl_mvcp 1
}

# подключаем функции _compl_... к нашим функциям
complete -o nospace -F _compl_cd cdd
complete -o nospace -F _compl_mvh cpd
complete -o nospace -F _compl_mvh mvd
complete -o nospace -F _compl_mvfh cpfd
complete -o nospace -F _compl_mvfh mvfd

