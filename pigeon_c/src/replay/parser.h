/*
 * parser.h
 *
 *  Created on: Aug 22, 2014
 *      Author: assertantawi
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <stdbool.h>

#define BRACKETS	" []{},"

/**
 * gets key1 and value1 in a string of the form:
 * key1=value1;key2=value;...;keyn=valuen
 * returns string starting with key2, or null if no more
 *
 * More generally, parse a sequence of key value pairs <Q> of
 * one or more key <K> value <V> pairs separated by ';',
 * where the value <V> could be a singleton string value or
 * a sequence of key value pairs <Q> enclosed in { }.
 *
 *<pre>
 * Vocabulary:
 * <S> := string of one or more characters, other than "=;{}\n"
 * <Q> := <K> = <V> | [ ; <Q> ]*
 * <K> := <S>
 * <V> := <S> | { <Q> }
 * </pre>
 */
extern char* parser_nextkeyvaluepair(char* Q, char* K, char* V, bool* issingleton);

/**
 * get the integer value in the string
 */
extern int parser_decode_int(char * s);

/**
 * get the integer array value in the string
 */
extern int** parser_decode_intarray(char * s, int nrows, int ncols);

/**
 * get the float value in the string
 */
extern float parser_decode_float(char * s);

/**
 * get the float array value in the string
 */
extern float** parser_decode_floatarray(char * s, int nrows, int ncols);

/**
 * get the float array value in the string
 * with variable number of columns, one value per row
 */
extern float** parser_decode_floatarrayvariable(char * s, int nrows, int * ncols);

/**
 * get the string value in the string
 */
extern char* parser_decode_string(char * s);

/**
 * get the string array value in the string
 */
extern char*** parser_decode_stringarray(char * s, int nrows, int ncols);

/**
 * get the string array value in the string
 * with variable number of columns, one value per row
 */
extern char*** parser_decode_stringarrayvariable(char * s, int nrows, int * ncols);

/**
 * get the boolean value in the string
 */
extern bool parser_decode_bool(char * s);

/**
 * get the boolean array value in the string
 */
extern bool** parser_decode_boolarray(char * s, int nrows, int ncols);


/**
 * encode "key=val;"
 */
extern char* parser_encode_int(char * key, int val);

/**
 * encode "key=[val_1,val_2,...,val_num];"
 */
extern char* parser_encode_intarray(char * key, int num, int* val);

/**
 * encode "key=[[val_11,...,val_1m]...[val_n1,...,val_nm]];"
 */
extern char* parser_encode_intmatrix(char * key, int nrows, int ncols, int** val);

/**
 * encode "key=val;"
 */
extern char* parser_encode_float(char * key, float val);

/**
 * encode "key=[val_1,val_2,...,val_num];"
 */
extern char* parser_encode_floatarray(char * key, int num, float* val);

/**
 * encode "key=[[val_11,...,val_1m]...[val_n1,...,val_nm]];"
 */
extern char* parser_encode_floatmatrix(char * key, int nrows, int ncols, float** val);

/**
 * encode "key=val;"
 */
extern char* parser_encode_bool(char * key, bool val);

/**
 * encode "key=[val_1,val_2,...,val_num];"
 */
extern char* parser_encode_boolarray(char * key, int num, bool* val);

/**
 * encode "key=val;"
 */
extern char* parser_encode_string(char * key, char* val);

/**
 * encode "key=[val_1,val_2,...,val_num];"
 */
extern char* parser_encode_stringarray(char * key, int num, char** val);

/**
 * encode "key=[[val_11,...,val_1m]...[val_n1,...,val_nm]];"
 */
extern char* parser_encode_stringmatrix(char * key, int nrows, int ncols, char*** val);


#endif /* PARSER_H_ */
