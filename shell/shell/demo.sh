args=( "$@" )
printf '%s\n' "${args[@]:1}"
unset args[0]
printf '%s\n' "${args[@]}"