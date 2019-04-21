#include <bits/stdc++.h>
#include <sys/wait.h>	
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
using namespace std;

// Os tres erros mais conmuns foram definidos como ERROR1, 2 e  ERROR3

// ERRO1 = Erro de alocação dinamica
// ERRO2 = Comando não foi reconhecido
// ERRO2 = Erro de diretorio


#define ERROR1 printf("Erro de alocação\n")
#define ERROR2 printf("%s : Comando não encontrado.\n",parametros[0])
#define ERROR3 printf("%s não é um diretorio válido.\n",parametros[1])

#define SPLIT_DEFINTIONS " \t\r\n\a "

// struct de um comando
struct Comand
{
	const char * name; // nome do comando
	void (*func)(char **); // ponteiro para a funçao do comando
};

#define qtd_comands 3

// funções persionalizadas

void cd	 (char ** parametros);
void exit(char ** parametros);
void help(char ** parametros);

// vetor de comandos persionalizados
Comand comands[] = 
{
	{	"cd",   &cd		},
	{	"exit", &exit	},
	{	"help", &help	}
	
};

void print_shell(char * buffer)
{	
	// getcwd é uma chamada de sistema para o diretorio atual do processo
	printf("Shell@Aed:%s > ", getcwd(buffer, PATH_MAX + 1));
}

// executa as chamadas de sistema
void exec(char ** parametros){
	int son_status = 0, pid = fork();
		
	if(!pid)
	{
		// verifica se wexecutou com sucesso ou não
		if(execvp(parametros[0], parametros) == -1)
			ERROR2;
		
		exit(0);
	}
	
	// Se ainda esta esperando o filho, e ele nao morreu por algum motivo, continua esperando
	while( waitpid(pid, &son_status, WUNTRACED) && !WIFEXITED(son_status) && !WIFSIGNALED(son_status));
	
	// waitpid -> espera o filho mudar de estado, WUNTRACED é usado para
	// retornar quando algum processo filho para
	
	// WIFEXITED retorna 1 se o processo filho terminou de forma normal 
	// WIFSIGNALED retorna 1 se o processo filho terminou por um signal
	
}

// verifica se existe uma função implementada, caso contrario tenta executar o exec
void run_comand(char ** parametros)
{
	if(!parametros)
		return ;
		
	for(int i = 0; i < qtd_comands ; i++)
		if(!strcmp(parametros[0], comands[i].name))
				return comands[i].func(parametros);
				
	exec(parametros);
}

// função que separa os argumentos do input
char ** decode_comand(char * buf){		
	int idx = 0;
	
	// matriz de char dinamica
	char ** split = new char*[strlen(buf)+1];
	for(size_t i = 0; i < strlen(buf); i++)
	{
		split[i] = new char[strlen(buf)+1];
		
		if(!split || !split[i])
			ERROR1,exit(0);
	}
	
	// splitando os comandos do input
	char * p = strtok(buf, SPLIT_DEFINTIONS);
	
	while(p)
	{
		split[idx++] = p;
		p = strtok(NULL, SPLIT_DEFINTIONS);
		// strtok é uma função do c que explita strings de acordo com o parametro
		
	} split[idx] = 0;
	
	return split;
}

void shell()
{
	
	char * buffer = new char[PATH_MAX+1];
	char * input = new char[PATH_MAX+1];
	
	while(1)
	{
		print_shell(buffer);
		scanf(" %[^\n]s", input);
		run_comand( decode_comand(input));
	}
}

int main()
{
	shell();
	return 0;
}

// ---------------------------------------------------------------------
void cd(char ** parametros)
{
	// se o endereço for valido, executa a chamada de sistema para ir pra lá
	if(parametros[1] == NULL || chdir(parametros[1]))
		ERROR3;
}

// encerra o shell
void exit(char ** parametros)
{
	exit(0);
}

// exibe os comandos 
void help(char ** parametros)
{
	printf("\nHelp(Comandos) :::::::::::::::::::::::::::::::::::\n\n");
	
	for(int i = 0; i < qtd_comands; i++)
		printf("\t-%s\n", comands[i].name);
		
	printf("\nDigite o comando, seus parametros e aperte ENTER !\n\n");
}
