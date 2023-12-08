#ifndef LABEL_H
#define LABEL_H

/** Creates a general purpose private label.
  * @returns the index of the label
  * */
int label_create();

/** @returns a static string representation of the label, e.g. .L1 */
char* label_name(int index);

/** Creates a private label for local string. The code is also generated.
 * @return the index of the string label
 * */
int string_create(const char* string, int reg);

#endif
