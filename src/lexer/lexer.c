/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlin <wlin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 18:07:03 by wlin              #+#    #+#             */
/*   Updated: 2024/09/26 12:31:32 by wlin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	add_token(t_token **token_lst, char *word, t_metachar metachar)
{
	t_token	*new_node;
	t_token	*head;

	head = *token_lst;
	if ((int)head->metachar == UNINITIALIZED)
	{
		head->word = word;
		head->metachar = metachar;
	}
	else
	{
		new_node = create_lst_node(word, metachar);
		lst_add_back(token_lst, new_node);
	}
	return (1);
}

int	handle_word(t_token **token_lst, char *input, int start)
{
	int		i;
	char	*word;

	i = start - 1;
	while (input[++i])
	{
		if (input[i] == QUOTE_D || input[i] == QUOTE_S)
		{
			i = find_matching_quote(input, i + 1, input[i]);
			if (i == NOT_FOUND)
			{
				ft_error(input, start);
				return (i);
			}
		}
		else if (is_delimiter(input[i]) == TRUE)
			break ;
	}
	word = ft_substr(input, (unsigned int)start, i - start);
	add_token(token_lst, word, NONE);
	return (i);
}

int	get_next_token(t_token **token_lst, char *input, int start)
{
	int	i;

	i = start;
	if (input[i] == C_PIPE)
		i += add_token(token_lst, NULL, PIPE);
	else if (input[i] == C_LESS && input[i + 1] != C_LESS)
		i += add_token(token_lst, NULL, LESS);
	else if (input[i] == C_LESS && input[i + 1] == C_LESS)
		i += add_token(token_lst, NULL, LESS_LESS) + 1;
	else if (input[i] == C_GREAT && input[i + 1] != C_GREAT)
		i += add_token(token_lst, NULL, GREAT);
	else if (input[i] == C_GREAT && input[i + 1] == C_GREAT)
		i += add_token(token_lst, NULL, GREAT_GREAT) + 1;
	else
		i = handle_word(token_lst, input, i);
	return (i);
}

int	tokenize(t_token **token_lst, char *input)
{
	int		i;

	i = 0;
	*token_lst = NULL;
	if (!input || *input == '\0')
		return (FALSE);
	*token_lst = create_lst_node(NULL, -1);
	if (*token_lst == NULL)
		return (FALSE);
	while (input[i])
	{
		if (is_whitespace(input[i]))
			i = skip_spaces(input, i);
		else
			i = get_next_token(token_lst, input, i);
		if (i < 0)
			return (FALSE);
	}
	return ((*token_lst)->metachar != UNSET);
}
