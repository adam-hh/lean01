#ifndef CALCULATOR_H
#define CALCULATOR_H

#ifdef __cpluscplus
extern "C" {
#endif

enum arithmeticOP {
  ADD = '+',
  SUB = '-',
  MLT = '*',
  DIV = '/',
  BRACKET1 = '(',
  BRACKET2 = ')',
  RET      = '\n'
};

enum arithmeticType {
  UNKNOWT   = '9',
  NUMBER    = '0',
  PAI       = '1',
  FUNCSIN   = '2',
  FUNCPOW   = '3'
};

extern const int arithPriority[];

void reverPoland(FILE *in, FILE *out);
void postToinfix(FILE *in, FILE *out);






#ifdef __cpluscplus
}
#endif

#endif    /* CALCULATOR_H */