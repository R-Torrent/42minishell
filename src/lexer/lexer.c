/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlin <wlin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 18:07:03 by wlin              #+#    #+#             */
/*   Updated: 2024/06/01 18:11:28 by wlin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"
#include "macros.h"

int	handle_tocken(char *input)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (is_whitespace(input[i]))
			i = skip_spaces(input, i);
		else if (input[i] == QUOTE_S)
		{}
		else if (input[i] == QUOTE_D)
		{}
		else if (input [i] == )

	}
}
