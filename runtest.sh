#!/bin/sh

export PATH=$PATH:.

if [ $# -lt 1 ]; then
    echo "Please specify the module to test."
    exit 1
fi

module="$1"

printer_test () {
  for testfile in ./test/printer/good*.bminor; do
    if bminor --print "$testfile" > "$testfile.out" &&
       bminor --print "$testfile.out" > "$testfile.tmp" &&
       diff "$testfile.out" "$testfile.tmp" > /dev/null; then
      echo "$testfile success (as expected)"
      rm "$testfile.tmp"
    else
      echo "$testfile failure (INCORRECT)"
    fi
  done
}

case $module in
    "encoder")
        command="encode"
        ;;
    "scanner")
        command="scan"
        ;;
    "parser")
        command="parse"
        ;;
    "printer")
        printer_test
        exit
        ;;
    "resolver")
        command="resolve"
        ;;
    *)
        echo "Unknown module: $module"
        exit 1
        ;;
esac

for testfile in ./test/"$module"/good*.bminor; do
  if bminor --"$command" "$testfile" > "$testfile.out"; then
    echo "$testfile success (as expected)"
  else
    echo "$testfile failure (INCORRECT)"
  fi
done

for testfile in ./test/"$module"/bad*.bminor; do
  if bminor --"$command" "$testfile" > "$testfile.out"; then
    echo "$testfile success (INCORRECT)"
  else
    echo "$testfile failure (as expected)"
  fi
done
