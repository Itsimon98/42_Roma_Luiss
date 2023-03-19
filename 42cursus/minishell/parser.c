/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eddy <eddy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 04:14:26 by eddy              #+#    #+#             */
/*   Updated: 2023/03/12 17:43:51 by eddy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "global.h"
#include "functions.h"

static void	fill_spaces(char *str);
static void	trim_spaces(char *str);
static void	squeez_spaces(char *str);
static void	expand_cmd(char **str, char *env[]);

/*
*/
int		parser(char *cmds, int *n_cmds, char *env[])
{
	int			i;
	int			j;
	int			ret;
	int			pid_ret;
	pid_t		pid;
	t_command	*commands;

	ret = 0;
	commands = malloc(sizeof(t_command) * (*n_cmds));
	if (!commands)
	{
		perror("Fail malloc\n");
		free(env);
		exit(1);
	}
	//TODO controlla le malloc sotto, exit(1) mi basta per non fare le free?
	i = -1;
	while (++i < *n_cmds)	//allocate memory for the commands[i] arrays
	{
		commands[i].args = malloc(sizeof(char *) * MAX_ENTRY);
		commands[i].rdr_in = malloc(sizeof(char *) * MAX_ENTRY);
		commands[i].rdr_out = malloc(sizeof(char *) * MAX_ENTRY);
		commands[i].heredoc = malloc(sizeof(char *) * MAX_ENTRY);
		commands[i].append = malloc(sizeof(char *) * MAX_ENTRY);
		j = -1;
		while (++j < MAX_ENTRY)	//allocate memory for the commands[i].arrays strings
		{
			commands[i].args[j] = malloc(sizeof(char) * MAX_NAME);
			commands[i].rdr_in[j] = malloc(sizeof(char) * MAX_NAME);
			commands[i].rdr_out[j] = malloc(sizeof(char) * MAX_NAME);
			commands[i].heredoc[j] = malloc(sizeof(char) * MAX_NAME);
			commands[i].append[j] = malloc(sizeof(char) * MAX_NAME);
			ft_memset(commands[i].args[j], '\0', MAX_NAME);
			ft_memset(commands[i].rdr_in[j], '\0', MAX_NAME);
			ft_memset(commands[i].rdr_out[j], '\0', MAX_NAME);
			ft_memset(commands[i].heredoc[j], '\0', MAX_NAME);
			ft_memset(commands[i].append[j], '\0', MAX_NAME);
		}
		commands[i].n_cmds = *n_cmds;
		commands[i].ret = 0;
		commands[i].err = 0;
	}
	i = -1;
	while (++i < *n_cmds)
	{
		if (fill_cmd(&commands[i], i, cmds) != 0)//TODO dividere anche in rdr e heredoc non tutto in args
		{
			perror("Fail fill_cmd\n");
			i = -1;
			while (++i < *n_cmds)	//free loop for memory in the commands[i] arrays
			{
				j = -1;
				while (++j < MAX_ENTRY)	//free loop for memory in the commands[i].arrays strings
				{
					free(commands[i].args[j]);
					free(commands[i].rdr_in[j]);
					free(commands[i].rdr_out[j]);
					free(commands[i].heredoc[j]);
					free(commands[i].append[j]);
				}
				free(commands[i].args);
				free(commands[i].rdr_in);
				free(commands[i].rdr_out);
				free(commands[i].heredoc);
				free(commands[i].append);
			}
			free(commands);
			free(env);
			exit(1);
		}
////////////////DEBUG CMDs
		if(0){
			int p=0;
			int sbit=0;
			while(p<10)
			{
				if(commands[i].args[p] && sbit==0)
					printf("args '%-10s' %d-%d\n",commands[i].args[p],i,p);
				else
					sbit=1;
				p++;
			}
			p=0;sbit=0;
			while(p<10)
			{
				if(commands[i].rdr_in[p] && sbit==0)
					printf("rdr_in '%-10s' %d-%d\n",commands[i].rdr_in[p],i,p);
				else
					sbit=1;
				p++;
			}
			p=0;sbit=0;
			while(p<10)
			{
				if(commands[i].rdr_out[p] && sbit==0)
					printf("rdr_out '%-10s' %d-%d\n",commands[i].rdr_out[p],i,p);
				else
					sbit=1;
				p++;
			}
			p=0;sbit=0;
			while(p<10)
			{
				if(commands[i].heredoc[p] && sbit==0)
					printf("heredoc '%-10s' %d-%d\n",commands[i].heredoc[p],i,p);
				else
					sbit=1;
				p++;
			}
			p=0;sbit=0;
			while(p<10)
			{
				if(commands[i].append[p] && sbit==0)
					printf("append '%-10s' %d-%d\n",commands[i].append[p],i,p);
				else
					sbit=1;
				p++;
			}
		}
////////////////
	}
////////////////DEBUG ENV VAR
	if(0){
		printf("\nVARIABILI D'AMBIENTE\n\n");
		int p = 0;
		while (env[p] != NULL) {
			printf("%s\n", env[p]);
			p++;
		}
	}
///////////////
	i = -1;
	int fd[*n_cmds - 1][2];
	// int fd1[2];

// 		commands->red_type = malloc(sizeof(int));
// 	commands->red_type[0] = 0;
// commands->red_type[1] = 0;
// commands->red_type[2] = 0;
	//   printf("%s", commands[0].args[0]);
	while (++i < *n_cmds)
	{//TODO non ho alcuna relazione tra esecuzione comando prima e dopo
		//TODO espando $
		//TODO levo gli apici dagli args
		ret = do_builtin(commands[i].args[0], commands[i].args, env, &commands);
		if ( ret == 2)	//il comando non e' builtin
		{	
			if(fd[i])	 
			pipe(fd[i]);
			pid = fork();
			if (pid == -1){
				perror("Fail fork\n");
				exit(1);
			}
			if (pid == 0) //processo figlio
			{ 
				// printf("executing here ");
			
			  	 if(i > 0)
			  	pipein(fd[i -1]); // prendo l'input dalla read della pipe precedente
				   if (i + 1 < *n_cmds)			// se come controllo uso commands[i].args non funziona sempre come condizione
				     pipeout(fd[i]);                    //TODO devo chiudere la read della pipe precedente
				   // printf("äoooo\n\n\\n");
				// redirect_i(commands);
				expand_cmd(&(commands[i].args[0]), env);
				execve(commands[i].args[0], commands[i].args, env);
				printf("bash: %s: command not found\n", commands[i].args[0]);
				exit(1);
			}
			else //processo padre
			{
				waitpid(pid, NULL, 0);
				  close(fd[i][1]);
				 if (!commands[i + 1].args) 
				    close(fd[i][0]);
				if (WIFEXITED(pid_ret) && WEXITSTATUS(pid_ret) == 1)//la execve fallisce e ritorna con exit(1)
					ret = 1;
				else
					ret = 0;
			}
		}
	}
	i = -1;
	while (++i < *n_cmds)	//free loop for memory in the commands[i] arrays
	{
		j = -1;
		while (++j < MAX_ENTRY)	//free loop for memory in the commands[i].arrays strings
		{
			free(commands[i].args[j]);
			free(commands[i].rdr_in[j]);
			free(commands[i].rdr_out[j]);
			free(commands[i].heredoc[j]);
			free(commands[i].append[j]);
		}
		free(commands[i].args);
		free(commands[i].rdr_in);
		free(commands[i].rdr_out);
		free(commands[i].heredoc);
		free(commands[i].append);
	}
	free(commands);
	return (ret);
}

/*
*/
int	fill_cmd(t_command *to_fill, int n_cmd, char *in_line)
{
	int			i;
	int			j;
	int			tmp[5];
	int			quote_rep;
	int			quotes_rep;
	char		cmd[MAX_CMD];
	

	tmp[0] = -1;
	quote_rep = 0;
	quotes_rep = 0;
	i = 0;
	j = 0;
	while (in_line[i] != '\0')
	{
		if (in_line[i] == '\'' && quotes_rep % 2 == 0)
			quote_rep++;
		if (in_line[i] == '"' && quote_rep % 2 == 0)
			quotes_rep++;
		if (tmp[0] == n_cmd - 1)
		{
			cmd[j] = in_line[i];
			j++;
		}
		if (in_line[i] == '|' && quote_rep % 2 == 0 && quotes_rep % 2 == 0)
			tmp[0]++;
		if (tmp[0] == n_cmd)
			break ;
		i++;
	}
	cmd[j] = '\0';
	if (setup_spaces(cmd) != 0)
		return (1);
	i = 0;
	j = 0;
	quote_rep = 0;
	quotes_rep = 0;
	tmp[0] = 0;
	tmp[1] = 0;
	tmp[2] = 0;
	tmp[3] = 0;
	tmp[4] = 0;
	while (cmd[i])
	//voglio che se sono simboli $ siano modificate
	//VOGLIO CHE SE SONO IN quotes restino uguali senza le estremita'
	{
		if (cmd[i] == '\'' && quotes_rep % 2 == 0)
			quote_rep++;
		if (cmd[i] == '"' && quote_rep % 2 == 0)
			quotes_rep++;
		if (cmd[i] == ' ' && quote_rep % 2 == 0 && quotes_rep % 2 == 0)
		{
			ft_strlcat(to_fill->args[tmp[0]], "\0", MAX_NAME);//non lo mette se sono alla fine perche non ho lo spazio
			(tmp[0])++;
			j = 0;
		}
		else
		{
			ft_strlcat(to_fill->args[tmp[0]], (char[]){cmd[i], '\0'}, MAX_NAME);//vado a inserire la singola lettera ma la vedo come stringa e non come char
			j++;
		}
		i++;
	}
	ft_strlcat(to_fill->args[tmp[0]], "\0", MAX_NAME);
	free(to_fill->args[tmp[0]+1]);
	to_fill->args[tmp[0]+1] = NULL;
	free(to_fill->rdr_in[tmp[1]+1]);
	to_fill->rdr_in[tmp[1]+1] = NULL;
	free(to_fill->rdr_out[tmp[2]+1]);
	to_fill->rdr_out[tmp[2]+1] = NULL;
	free(to_fill->heredoc[tmp[3]+1]);
	to_fill->heredoc[tmp[3]+1] = NULL;
	free(to_fill->append[tmp[4]+1]);
	to_fill->append[tmp[4]+1] = NULL;
	return (0);
}

/*
*/
int	setup_spaces(char *str)
{
	fill_spaces(str);
	trim_spaces(str);
	squeez_spaces(str);
	return (0);
}

/*
*/
static void	fill_spaces(char *str)
{
	int		i;
	int		j;
	int		len;
	char	new_str[MAX_CMD];

	len = ft_strlen(str);
	i = 0;
	j = 0;
	while (i < len)
	{
		if (str[i] == '<' || str[i] == '>' || str[i] == '|') {
			new_str[j++] = ' ';
			new_str[j] = str[i];
			if (str[i+1] == str[i]) { // if the next character is also a '<', '>', or '|'
				new_str[++j] = str[i+1];
				i++; // skip the next character
			}
			new_str[++j] = ' ';
		} else if (str[i] == '"' || str[i] == '\'') {
			char quote = str[i];
			new_str[j] = str[i];
			while (i < len && str[++i] != quote) {
				new_str[++j] = str[i];
			}
			new_str[++j] = quote;
		} else {
			new_str[j] = str[i];
		}
		i++;
		j++;
	}
	new_str[j] = '\0';
	i = 0;
	while (new_str[i] != '\0')//loop per ricopiare la stringa
	{
		str[i] = new_str[i];
		i++;
	}
	str[i] = '\0';
}

/*
*/
static void	trim_spaces(char *str)
{
	int		i;
	int		j;
	int		k;
	int		len;
	int		in_word;
	int		in_quote;
	char	quote_char;

	len = ft_strlen(str);
	i = 0;
	while (i < len && ft_isspace(str[i])) {
		i++;
	}
	j = len - 1;
	while (j >= 0 && ft_isspace(str[j])) {
		j--;
	}
	k = 0;
	in_word = 0;
	in_quote = 0;
	quote_char = '\0';
	while (i <= j)
	{
		if (str[i] == '\'' || str[i] == '\"')
		{
			if (!in_quote) {
				in_quote = 1;
				quote_char = str[i];
			} else if (str[i] == quote_char) {
				in_quote = 0;
				quote_char = '\0';
			}
			str[k++] = str[i];
		} else if (!ft_isspace(str[i]) || (in_quote && ft_isspace(str[i]))) {
			str[k++] = str[i];
			in_word = 1;
		} else if (in_word) {
			str[k++] = ' ';
			in_word = 0;
		}
		i++;
	}
	if (str[k - 2] == ' ' && str[k - 1] == '|')
		str[k - 2] = '\0';	
	if (str[k - 1] == '|')
		str[k - 1] = '\0';
	str[k] = '\0';
}

/*
*/
static void	squeez_spaces(char *str)
{
	int i;
	int j;
	int n;

	n = ft_strlen(str);
	i = 0;
	j = 0;
	while (i < n)
	{
		if (str[i] == '>' || str[i] == '<') {
			str[j++] = str[i++];
			if (i < n && (str[i] == '>' || str[i] == '<')) {
				str[j++] = str[i++];
			}
			while (i < n && ft_isspace(str[i])) {
				i++;
			}
		}
		else
		{
			str[j++] = str[i++];
		}
	}
	str[j] = '\0';
}

/*
*/
static void	expand_cmd(char **str, char *env[])//TODO non posso usare queste funzioni, pero funziona
{
    char* cmd = *str;
    char* path_env = adhoc_getenv("PATH", env);
    
    
    if (!path_env) {// If the PATH environment variable isn't set, return
        return;
    }
    
    // Copy the PATH environment variable to a new buffer so that we can modify it
    char path[MAX_NAME];
    strcpy(path, path_env);
    
    // Tokenize the PATH variable by ':' characters
    char* dir = strtok(path, ":");
    while (dir) {
        // Create a buffer for the full path to the command in this directory
        char full_path[MAX_NAME];
        strcpy(full_path, dir);
        strcat(full_path, "/");
        strcat(full_path, cmd);
        
        // Check if the command exists in this directory
        if (access(full_path, X_OK) == 0) {
            // If it does, replace the command string with the full path
            *str = strdup(full_path);
            return;
        }
        
        // If not, move on to the next directory in the PATH
        dir = strtok(NULL, ":");
    }
}
