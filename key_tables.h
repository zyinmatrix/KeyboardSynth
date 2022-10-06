#ifndef _KEYTABLE_H_
#define _KEYTABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

bool valid_key(int key);
double key2freq(int key);
void init_key2index(void);

#ifdef __cplusplus
}
#endif

#endif /* _KEYTABLE_H_ */
