#!/bin/bash

SSH_DIR=$HOME/.ssh/
AUTH_KEYS_DIR=${SSH_DIR}/authorized_keys.d
AUTH_KEYS_FILE=${SSH_DIR}/authorized_keys

# Utils

key-path() {
  echo "${AUTH_KEYS_DIR}/${1}"
}

update-auth-keys() {
  cat ${AUTH_KEYS_DIR}/* > ${AUTH_KEYS_FILE}
}

log() {
  echo $@ >&2
}

# Commands

cmd:add() {
  if [ -z "$1" ]
  then
    log "Key name is required"
    exit 1
  fi
  local KEY=`key-path $1`
  if [ -f "$KEY" ]
  then
    log "Key '$1' already exists"
    exit 1
  fi
  # Determine is there a STDIN input or not.
  if [ -t 0 -a -t 1 ]
  then
    # Use default editor to edit key
    editor $KEY
  else
    cat /dev/stdin > $KEY
  fi
  update-auth-keys
}

cmd:edit() {
  if [ -z "$1" ]
  then
    log "Key name is required"
    exit 1
  fi
  local KEY=`key-path $1`
  # Determine is there a STDIN input or not.
  if [ -t 0 -a -t 1 ]
  then
    # Use default editor to edit key
    editor $KEY
  else
    cat /dev/stdin > $KEY
  fi
  update-auth-keys
}

cmd:remove() {
  if [ -z "$1" ]
  then
    log "Key name is required"
    exit 1
  fi
  local KEY=`key-path $1`
  if [ -f "$KEY" ]
  then
    rm "$KEY"
    update-auth-keys
  fi
}

cmd:has() {
  local KEY=`key-path $1`

  if [ -f "$1" ]
  then
    exit 0
  else
    exit 1
  fi
}

cmd:list() {
  for KEY in `ls $AUTH_KEYS_DIR`
  do
    echo $KEY
  done
}

cmd:help() {
  log "Usage is:"
  log "  add <key> - Add key"
  log "  edit <key> - Edit existing key"
  log "  remove <key> - Remove key"
  log "  has <key> - Determine if key exists"
  log "  list - List keys"
  log "  help - Output this message"
}

case $1 in
  add|edit|remove|has|list|help)
    cmd:$@
    ;;
  *)
    cmd:help
    exit 1
    ;;
esac
