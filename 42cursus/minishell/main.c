/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ecoli <ecoli@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 04:14:26 by eddy              #+#    #+#             */
/*   Updated: 2023/02/13 20:10:10 by ecoli            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "global.h"
#include "functions.h"

static void	handler(int sig, siginfo_t *siginfo, void *context);
static void	do_ps1(char *ps1, size_t len, int mod);
static int	insert_ps1(char *ps1, int p, const char *color);
static int	is_empty(char *str);

/*
WHERE:
struct sigaction {
	void (*sa_handler)(int);
	void (*sa_sigaction)(int, siginfo_t *, void *);
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_restorer)(void);
}
*/
int	main(int argc, char *argv[], char *envp[])
{
	struct sigaction	act;
	char				*in_line;
	char				ps1[2048];
	int					ret_val;
	int					n_cmd;

	ft_memset(&act, '\0', sizeof(act));
	act.sa_sigaction = &handler;				//Meaningful only when estabilishing a signal handler. More in 'man sigaction'
	act.sa_flags = SA_SIGINFO;					//When the SA_SIGINFO flag is specified in act.sa_flags, the signal handler address is passed via the act.sa_sigaction field. This handler takes three arguments. More in 'man sigaction'
	if (sigaction(SIGINT, &act, NULL) == -1)	//Examine and change a signal action
	{
		printf("Sigaction(SIGINT) fail\n");
		return (1);
	}
	if (sigaction(SIGQUIT, &act, NULL) == -1)	//Examine and change a signal action
	{
		printf("Sigaction(SIGQUIT) fail\n");
		return (1);
	}
	do_ps1(ps1, 2048, 0);
	while ( (in_line = readline(ps1)) != NULL)//per modificare questo faccio una stringa e metto quella
	{
printf("Ricevo = %s\n",in_line);
		if (!is_empty(in_line))
		{
			add_history(in_line);
			ret_val = analyzer(in_line, &n_cmd);
			if (ret_val == 0)
				ret_val = parser(in_line, &n_cmd, envp);
		}
		else
			ret_val = 0;
		free(in_line);
		do_ps1(ps1, 2048, ret_val);
	}
	printf("\n");//readline fallisce quando si preme ctrl-D (EOF) ergo usciamo dal while
	return (0);
}

/*
sig: Signal to handle.
siginfo: Buinld-in structure.
context:
Manages the signals received by the process
WHERE:
typedef struct {
	int si_signo;
	int si_code;
	union sigval si_value;
	int si_errno;
	pid_t si_pid;
	uid_t si_uid;
	void *si_addr;
	int si_status;
	int si_band;
} siginfo_t;
*/
static void	handler(int sig, siginfo_t *siginfo, void *context)
{
	(void)context;								//Cast for unused warning
	if (sig == SIGINT)//codice 2
	{
		printf("Premuto ctrl-C\n");//da cambiare con quello che devono effetivamente fare
	}
	if (sig == SIGQUIT)//codice 3
	{
		printf("Premuto ctrl-\\\n");//da cambiare con quello che devono effetivamente fare
	}
}

/*
*/
static int	is_empty(char *str)
{
	int i;

	i = 0;
	while (str[i] != '\0')
	{
		if (!ft_isspace(str[i]))
			return (0);
		i++;
	}
	return (1);
}

/*
*/
static void	do_ps1(char *ps1, size_t len, int mod)
{
	char	*pwd;
	char	*user;
	char	*desktop_session;
	int		i;
	int		j;

	ft_memset(ps1, '\0', len);
	user = getenv("USER");
	pwd = getenv("PWD");
	i = -1;
	if (mod == 0)
		i += insert_ps1(ps1, i + 1, "\U0001F607");//emoji angel face
	else
		i += insert_ps1(ps1, i + 1, "\U0001F608");//emoji devil face 
	i += insert_ps1(ps1, i + 1, "\033[1;32m");//grassetto e colore verde
	j = -1;
	while (++i < len && user[++j] != '\0')
		ps1[i] = user[j];
	ps1[i] = '@'; //cosi non devo fare -1 a i
	j = -1;
	i += insert_ps1(ps1, i, "\033[1;00m");//grassetto e colore bianco
	ps1[i] = ':';
	i += insert_ps1(ps1, i + 1, "\033[1;34m");//grassetto e colore blu
	j = -1;
	while (++i < len && pwd[++j] != '\0')
		ps1[i] = pwd[j];
	i += insert_ps1(ps1, i, "\033[1;00m");//grassetto e colore bianco
	ps1[i] = '$';
	ps1[i+1] = ' ';
}

/*
*/
static int insert_ps1(char *ps1, int p, const char *color)
{
	int i;

	i = -1;
	while (color[++i] != '\0')
		ps1[p + i] = color[i];
	return (i);
}
