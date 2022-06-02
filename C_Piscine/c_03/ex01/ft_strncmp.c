/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ecoli <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/09 10:42:23 by ecoli             #+#    #+#             */
/*   Updated: 2021/12/09 16:43:37 by ecoli            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_strncmp(char *s1, char *s2, unsigned int n)
{
	unsigned int	i;

	i = 0;
	if (n == 0)
			return (0);
	while (s1[i] == s2[i] &&s1[i] != '\0' && (i < n -1))
		i++;
	return (s1[i] - s2[i]);
}
