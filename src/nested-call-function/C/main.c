/* @TODO: Precisa ser atualizado. */
/*
Este módulo contém uma função main, e funções para operações aritméticas que aceitam 2 parâmetros inteiros retornando o resultado da operação.
Será gerado um código em LLVM como este em C:

int leiaInteiro();
void escrevaInteiro(int pi);

int add(int s, int t){
	return s + t;
}

int sub(int u, int v){
	return u - v;
}

int mul(int x, int w){
	return x * w;
}

int div(int y, int z){
	return y / z;
}

int main(){
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;
	int res = 0;
	int i;

	i = 0;

    do {
        a = leiaInteiro();
        b = leiaInteiro();
        c = leiaInteiro();
        d = leiaInteiro();
        res = add(add(mul(a,b),div(a,b)), sub(d,c));
        escrevaInteiro(res);
        i = i + 1;
	} while (i < 5);

    return(0);
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <llvm-c/Core.h>
#include <llvm-c/BitWriter.h>

int main(int argc, char *argv[]) {
  LLVMContextRef context = LLVMGetGlobalContext();
  LLVMModuleRef module = LLVMModuleCreateWithNameInContext("nested-call-function.bc", context);
  LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);

  // criação da função para adicionar ao módulo.
  LLVMTypeRef addParams[] = {LLVMInt64Type(), LLVMInt64Type()};
  LLVMTypeRef addFuncType = LLVMFunctionType(LLVMInt64Type(), addParams, 2, 0);
  LLVMValueRef soma = LLVMAddFunction(module, "soma", addFuncType); 

  // criação de um bloco básico para adicionar o código principal da função "soma"
  LLVMBasicBlockRef entryFnSoma = LLVMAppendBasicBlock(soma, "entry");

  // Declara o bloco de saída.
  LLVMBasicBlockRef exitFnSoma = LLVMAppendBasicBlock(soma, "exit");
  
  LLVMPositionBuilderAtEnd(builder, entryFnSoma);

  // LLVMValueRef LLVMGetParam(LLVMValueRef Fn, unsigned Index);
  LLVMValueRef tmp = 
  LLVMBuildAdd(builder, LLVMGetParam(soma, 0), LLVMGetParam(soma, 1), "tmp");

  // Cria um salto para o bloco de saída.
  LLVMBuildBr(builder, exitFnSoma);

  // Adiciona o bloco de saída.
  LLVMPositionBuilderAtEnd(builder, exitFnSoma); 
  // LLVMValueRef tmp1 = LLVMBuildLoad(builder, tmp, "ret");

  LLVMBuildRet(builder, tmp);

  // Cria um valor zero para colocar no retorno para o main.
  LLVMValueRef Zero64 = LLVMConstInt(LLVMInt64Type(), 0, false);

  // Declara o tipo do retorno da função main.
  LLVMTypeRef mainFnReturnType = LLVMInt64TypeInContext(context);
  // Cria a função main.
  LLVMValueRef mainFn = LLVMAddFunction(module, "main", LLVMFunctionType(mainFnReturnType, NULL, 0, 0));

  // Declara o bloco de entrada.
  LLVMBasicBlockRef entryBlock = LLVMAppendBasicBlockInContext(context, mainFn, "entry");
  // Declara o bloco de saída.
  LLVMBasicBlockRef exitBasicBlock = LLVMAppendBasicBlock(mainFn, "exit");

  // Adiciona o bloco de entrada.
  LLVMPositionBuilderAtEnd(builder, entryBlock);

  // Cria o valor de retorno e inicializa com zero.
  LLVMValueRef returnVal = LLVMBuildAlloca(builder, LLVMInt64Type(), "retorno");
	LLVMBuildStore(builder, Zero64, returnVal);

  // Declara as variáveis a,b e c.
  LLVMValueRef a = LLVMBuildAlloca(builder, LLVMInt64Type(), "a");
  LLVMValueRef b = LLVMBuildAlloca(builder, LLVMInt64Type(), "b");
  
  // Inicializa as variáveis.
  LLVMBuildStore(builder, LLVMConstInt(LLVMInt64Type(), 1, false), a);
  LLVMBuildStore(builder, LLVMConstInt(LLVMInt64Type(), 2, false), b);

  LLVMValueRef args[2] = {
       LLVMBuildLoad(builder, a, ""),
       LLVMBuildLoad(builder, b, "")
  };

  LLVMValueRef res = LLVMBuildCall(
        builder,
        soma,
        args,
        2,
        "res"
  );

  LLVMBuildStore(builder, res, returnVal);

  // Cria um salto para o bloco de saída.
	LLVMBuildBr(builder, exitBasicBlock);
	
	// Adiciona o bloco de saída.
	LLVMPositionBuilderAtEnd(builder, exitBasicBlock);
  
  // Cria o return.
	LLVMBuildRet(builder, LLVMBuildLoad(builder, returnVal, ""));

	// Imprime o código do módulo.
  LLVMDumpModule(module);

  // Escreve para um arquivo no formato bitcode.
  if (LLVMWriteBitcodeToFile(module, "meu_modulo.bc") != 0) {
    fprintf(stderr, "error writing bitcode to file, skipping\n");
  }
}
