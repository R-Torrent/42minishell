/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlin <wlin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/21 14:12:10 by wlin              #+#    #+#             */
/*   Updated: 2024/07/21 14:12:24 by wlin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "macros.h"

void    add_cmd_str(char *str, int i, t_commands *cmd)
{
    cmd->args[i] = ft_strdup(str);
    // printf("In add_cmd_str() => cmd.args[%d] = %s\n", i, cmd->args[i]);
}

void    add_cmd_redirect(t_metachar type, char *filename, t_commands *cmd)
{
   t_redirect   *new_redirect;
   t_redirect   *tmp_redirect;
   t_commands   *tmp;

   new_redirect = malloc(sizeof(t_redirect));
   new_redirect->type = type;
   new_redirect->filename = ft_strdup(filename);
   new_redirect->next = NULL;
   if (!cmd->redirect)
        cmd->redirect = new_redirect;
    else
    {
        tmp = cmd;
        tmp_redirect = tmp->redirect;
        while (tmp_redirect->next)
            tmp_redirect = tmp_redirect->next;
        tmp_redirect->next = new_redirect;
    }
}

int    count_cmd_str(t_token *tokens)
{
    t_token *tmp;
    int     len;

    tmp = tokens;
    len = 0;

    while (tmp && tmp->metachar != PIPE)
    {
        if (tmp->metachar > PIPE)
            tmp = tmp->next;
        else
            ++len;
        tmp = tmp->next;
    }
    return (len);
}

t_commands  *create_cmd_lstnew()
{
    t_commands  *new;

    new = malloc(sizeof(t_commands));
    if (!new)
        return (NULL);
    new->args = NULL;
    new->redirect = NULL;
    new->prev = NULL;
    new->next = NULL;
    return (new);
}

void    cmd_lst_addback(t_commands **cmds, t_commands *new)
{
    t_commands  *tmp;

    if (*cmds != NULL)
    {
        tmp = *cmds;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new;
        new->prev = tmp;
    }
    else
    {
        (*cmds) = new;
    }
}

t_commands *build_cmd(t_token **token_lst)
{
    int     i;
    int     len;
    t_commands  *cmd;

    cmd = create_cmd_lstnew();
    len = count_cmd_str(*token_lst);
    if (len > 0)
    {
        cmd->args = malloc(sizeof(char *) * (len + 1));
        if (!cmd->args)
            free_array(cmd->args);
    }
    i = -1;
    while (*token_lst && (*token_lst)->metachar != PIPE)
    {
        if (token_lst && (*token_lst)->metachar > PIPE)
        {
            add_cmd_redirect((*token_lst)->metachar, (*token_lst)->next->word, cmd);
            *token_lst = (*token_lst)->next;
        }
        else if (token_lst && (*token_lst)->word)
        {
            ++i;
            add_cmd_str((*token_lst)->word, i, cmd);
        }
        *token_lst = (*token_lst)->next;
    }
    if (cmd->args != NULL)
        cmd->args[len] = NULL;
    return (cmd);
}

int is_redirection(t_metachar type)
{
    if (type == LESS || type == LESS_LESS 
            || type == GREAT || type == GREAT_GREAT)
        return (1);
    else
        return (0);           
}

int    prompt_error_message(t_metachar type)
{
    if (type == PIPE)
        printf("minishell: syntax error near unexpected token `%c'\n", C_PIPE);
    else if (type == LESS)
        printf("minishell: syntax error near unexpected token `%c'\n", C_LESS);
    else if (type == LESS_LESS)
        printf("minishell: syntax error near unexpected token `%c%c'\n", C_LESS
            , C_LESS);
    else if (type == GREAT)
        printf("minishell: syntax error near unexpected token `%c'\n", C_GREAT);
    else if (type == GREAT_GREAT)
        printf("minishell: syntax error near unexpected token`%c%c'\n", C_GREAT
            , C_GREAT);
    return (EXIT_FAILURE);
}

int validate_cmd_syntax(t_token *token_lst)
{
    t_token *tmp;

    tmp = token_lst;
    while (tmp)
    {
        if (tmp->metachar == PIPE)
        {
            if (tmp->prev == NULL || tmp->next->metachar == PIPE)
                return (prompt_error_message(PIPE));
            else if (is_redirection(tmp->next->metachar) && tmp->next->next == NULL)
                return (printf("minishell: syntax error near unexpected token`newline'\n"), EXIT_FAILURE);
            else if (tmp->next->word && tmp->next->next == NULL)
                return (prompt_error_message(PIPE));
        }
        else if (is_redirection(tmp->metachar))
        {
            if (tmp->next == NULL)
                return (printf("minishell: syntax error near unexpected token`newline'\n"), EXIT_FAILURE);
            else if (tmp->next->metachar == PIPE)
                return (prompt_error_message(PIPE));
            else if (is_redirection(tmp->next->metachar) && tmp->metachar == tmp->next->metachar)
                return (prompt_error_message(tmp->metachar));
            else if (is_redirection(tmp->next->metachar) && tmp->metachar != tmp->next->metachar)
                return (prompt_error_message(tmp->next->metachar));
        }
        tmp = tmp->next;
    }
    return (EXIT_SUCCESS);
}

t_commands  *parse_tokens(t_token *tokens)
{
    t_commands  *cmds;
    t_commands  *new;

    if (validate_cmd_syntax(tokens) == EXIT_FAILURE)
        return (NULL);
    cmds = NULL;
    while (tokens)
    {
        new = build_cmd(&tokens);
        cmd_lst_addback(&cmds, new);
        if (tokens && tokens->metachar == PIPE)
            tokens = tokens->next;
    }
    return (cmds);
}