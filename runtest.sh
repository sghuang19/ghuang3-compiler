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

codegen_test () {
  for testfile in ./test/codegen/good*.bminor; do
    if bminor --codegen "$testfile" > "$testfile.out" &&
       gcc -x assembler "$testfile.out" -o "$testfile.exe"; then
      echo "$testfile success (as expected)"
    else
      echo "$testfile failure (INCORRECT)"
    fi
    rm "$testfile.exe"
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
    "typechecker")
        command="typecheck"
        ;;
    "codegen")
        codegen_test
        exit
        ;;
    *)
        echo "Unknown module: $module"
        exit 1
        ;;
esac

for testfile in ./test/"$module"/good*.bminor; do
  if bminor --"$command" "$testfile" > "$testfile.out" 2>&1; then
    echo "$testfile success (as expected)"
  else
    echo "$testfile failure (INCORRECT)"
  fi
done

for testfile in ./test/"$module"/bad*.bminor; do
  if bminor --"$command" "$testfile" > "$testfile.out" 2>&1; then
    echo "$testfile success (INCORRECT)"
  else
    echo "$testfile failure (as expected)"
  fi
done
