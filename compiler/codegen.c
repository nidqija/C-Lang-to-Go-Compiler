#include <stdio.h>
#include "../include/codegen.h"
#include <string.h>

// function to recursive traversal AST nodes and emits equivalent go code to output file
static void emit_node(const ASTNode *node , FILE *out){

    // if there is no nodes being returned , do nothing and return
    if(!node) return;

    // switch statements to identify the types of nodes being received 
    switch(node->kind){

        // if node received is integer type , write its raw numeric value
        case NODE_INT_LITERAL:
             fprintf(out , "%lld" , (long long)node->int_literal.value);
             break;

        case NODE_STRING_LITERAL:
            fprintf(out , "%.*s", (int)node->string_literal.length, node->string_literal.value);
            break;

        // if node received is a return statement 
        // emit the go return keyword , its expression value and a newline
        case NODE_RETURN_STMT:
            fprintf(out , "\treturn ");
            emit_node(node->return_stmt.expr, out);
            fprintf(out , "\n");
            break;


        case NODE_EXPR_STMT:
            fprintf(out , "\t");
            emit_node(node->return_stmt.expr , out);
            fprintf(out , "\n");
            break;

        case NODE_CALL_STMT:
            if(node->call_expr.name_len == 6 && strncmp(node->call_expr.name, "printf" , 6) == 0){
                fprintf(out , "fmt.Printf(");
            } else {
                fprintf(out, "%.*s(", (int)node->call_expr.name_len, node->call_expr.name);

            }

            for(size_t i = 0 ; i < node->call_expr.arg_count; i++){
                emit_node(node->call_expr.args[i], out);

                if(i + 1 < node->call_expr.arg_count){
                    fprintf(out , ", ");
                }
            }

            fprintf(out , ")");
            break;


        // if node received is function declaration , 
        // check if node is a main function or just a helper function
        case NODE_FUNCTION_DECL: {

            // check if the main function has a length of 4 and matched characters
            int is_main = (node->func_decl.name_len == 4 &&
                    strncmp(node->func_decl.name, "main" , 4) == 0);


            
            // if it is a main function, print the required code coversion to go code 
            if (is_main){
                fprintf(out , "package main\n\n");
                fprintf(out , "import \"fmt\"\n");
                fprintf(out , "import \"os\"\n\n");
                fprintf(out, "func cMain() int32{\n");

                for(size_t i =0; i < node->func_decl.body_count ; i++){
                    emit_node(node->func_decl.body[i] , out);
                }


                fprintf(out , "}\n\n");
                fprintf(out , "func main(){\n");
                fprint(out , "\t_ = fmt.Printf\n");
                fprintf(out , "\tos.Exit(int(cMain()))\n");
                fprintf(out , "}\n");
            } else {
                fprintf(out, "func %.*s() int32 {\n",
                    (int)node->func_decl.name_len , node->func_decl.name);

                for (size_t i = 0 ; i < node->func_decl.body_count ; i++){
                    emit_node(node->func_decl.body[i] , out);
                }

                fprintf(out , "}\n");
            }

            break;
            
        }

        // if node received is a binary expression , 
        // we will generate the expressions in go code by checking the 
        // left side of the nodes ,
        //  then to the right according to PEMDAS formulations
        case NODE_BINARY_EXPR: {
            emit_node(node->binary_expr.left , out);

            const char *op_str = "?";

            switch(node->binary_expr.op){
                case TOKEN_PLUS:
                  op_str = "+";
                  break;

                case TOKEN_MINUS:
                  op_str = "-";
                  break;

                case TOKEN_MULTI:
                  op_str = "*";
                  break;

                case TOKEN_DIVIDE:
                  op_str = "/";
                  break;

                default:
                  break;
            }

            fprintf(out , " %s " , op_str);

            emit_node(node->binary_expr.right , out);
            break;
        }
    }
}

void codegen_go(const ASTNode *root , FILE *out){
    emit_node(root, out);
}