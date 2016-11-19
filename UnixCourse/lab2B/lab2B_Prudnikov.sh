
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


_compl_cdd()
{
    #set -xue
    local cur
    COMPREPLY=()
    # текущее слово в консоли
    cur="${COMP_WORDS[COMP_CWORD]}"

    # директория + подкаталог
    impDir="$HOME"/Downloads/"$cur"

    local tags
    # генерация  тэгов дополнения 
    if [[ "$cur" == *"/"* ]] 
    then 
        tags="$cur"$(compgen -d -- "$impDir" | awk -F "/" '{ print $NF"/" }')
    else
        tags=$(compgen -d -- "$impDir" | awk -F "/" '{ print $NF"/" }')
    fi

    # добавление в массив вариантов дополнения текущего слова  
    COMPREPLY=( ${tags} )
}

_compl_cp_mv_d()
{
    #set -xue
    local cur
    COMPREPLY=()
    # текущее слово в консоли
    cur="${COMP_WORDS[COMP_CWORD]}"

    # директория + подкаталог
    # если вводится первый аргумент
    if [[ ${COMP_CWORD} == 1 ]] ; then
        impDir="$cur"
    # если второй
    elif [[ ${COMP_CWORD} == 2 ]]; then
        impDir="$HOME"/Downloads/"$cur"
    fi

    local tags
    # генерация  тэгов дополнения 
    if [[ "$cur" == *"/"* ]]; then 
        tags="$cur"$(compgen -d -- "$impDir" | awk -F "/" '{ print $NF"/" }')
    else
        tags=$(compgen -d -- "$impDir" | awk -F "/" '{ print $NF"/" }')
    fi

    # добавление в массив вариантов дополнения текущего слова  
    COMPREPLY=( ${tags} )
}

_compl_cpf_mvf_d()
{
    #set -xue
    local cur
    COMPREPLY=()
    # текущее слово в консоли
    cur="${COMP_WORDS[COMP_CWORD]}"

    # директория + подкаталог
    # если вводится первый аргумент
    if [[ ${COMP_CWORD} == 2 ]] ; then
        impDir="$cur"
    # если второй
    elif [[ ${COMP_CWORD} == 1 ]]; then
        impDir="$HOME"/Downloads/"$cur"
    fi

    local tags
    # генерация  тэгов дополнения 
    if [[ "$cur" == *"/"* ]]; then 
        tags="$cur"$(compgen -d -- "$impDir" | awk -F "/" '{ print $NF"/" }')
    else
        tags=$(compgen -d -- "$impDir" | awk -F "/" '{ print $NF"/" }')
    fi

    # добавление в массив вариантов дополнения текущего слова  
    COMPREPLY=( ${tags} )
}


# подключаем функции _compl_... к нашим функциям
complete -o nospace -F _compl_cdd cdd
complete -o nospace -F _compl_cp_mv_d cpd
complete -o nospace -F _compl_cp_mv_d mvd
complete -o nospace -F _compl_cpf_mvf_d cpfd
complete -o nospace -F _compl_cpf_mvf_d mvfd

