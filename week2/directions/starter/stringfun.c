#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int max_len) {
    char *src = user_str;
    char *dst = buff;
    int processed_len = 0;
    int whitespace_flag = 0;

    // Ensure there is space for at least brackets and one character
    if (max_len < 3) {
        return -1;  // Not enough space for brackets
    }

    // Add the opening bracket at the beginning
    *dst++ = '[';
    processed_len++;

    // Skip leading spaces
    while (*src == ' ' || *src == '\t') {
        src++;
    }

    while (*src != '\0') {
        if (*src == ' ' || *src == '\t') {
            // Only add one space between words
            if (!whitespace_flag && processed_len < max_len - 2) { // Reserve space for closing bracket
                *dst++ = ' ';
                processed_len++;
                whitespace_flag = 1;
            }
        } else {
            // Add non-space characters to the buffer
            if (processed_len < max_len - 2) { // Reserve space for closing bracket
                *dst++ = *src;
                processed_len++;
            } else {
                return -1;  // Error: String too large for buffer
            }
            whitespace_flag = 0;
        }
        src++;
    }

    // Skip trailing spaces before finishing
    while (processed_len > 1 && *(dst - 1) == ' ') {
        dst--;  // Remove the last space
        processed_len--;
    }

    // Fill the rest of the buffer with dots
    while (processed_len < max_len - 1) { // Reserve space for closing bracket
        *dst++ = '.';
        processed_len++;
    }

    // Add the closing bracket at the end
    *dst++ = ']';
    processed_len++;

    return processed_len;
}


void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int word_count = 0;
    int in_word = 0; // Flag to track if we are inside a word

    for (int i = 0; i < str_len; i++) {
        // Check if the current character is non-whitespace
        if (*(buff + i) != ' ') {
            if (!in_word) {
                // Transitioning from whitespace to a word
                word_count++;
                in_word = 1;
            }
        } else {
            // If the current character is whitespace, exit the word
            in_word = 0;
        }
    }

    return word_count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

    /*
     * WHY IS THIS SAFE?
     * -----------------
     * The condition (argc < 2) ensures that argv[1] is not accessed if it does not exist.
     * 
     * If the user does not provide any arguments beyond the program name, argc will be 1,
     * meaning argv[1] does not exist. Attempting to dereference argv[1] in this case
     * would result in an error. By checking (argc < 2) first, the code avoids
     * accessing argv[1] when it is invalid.
     * 
     * Additionally, the || operator uses short-circuit evaluation, meaning the second condition
     * (*argv[1] != '-') is only evaluated if (argc < 2) is false. This guarantees that
     * argv[1] is valid before it is dereferenced.
     */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*
     * This if statement ensures that the program has received at least two command-line arguments
     * (in addition to the program name). The purpose is to validate that the user has provided
     * the required arguments for proper execution. If fewer than two arguments are supplied:
     * - The `usage()` function is called to inform the user about the correct way to invoke the program.
     *  - The program exits with a status of 1 to indicate improper usage.
     *
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = (char *)malloc(BUFFER_SZ);
    if (buff == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(2);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options

        case 'r': {
            // Skip the opening bracket and exclude the dots and closing bracket
            char *start = buff + 1;  // Skip the opening bracket '['
            char *end = buff + user_str_len - 2;  // Skip the closing bracket ']'

            // Reverse only the meaningful part of the string (exclude dots and brackets)
            while (start < end) {
                char temp = *start;
                *start++ = *end;
                *end-- = temp;
            }

            // Ensure the buffer has the opening bracket '[' at the beginning and closing bracket ']' at the end
            buff[0] = '[';
            buff[user_str_len - 1] = ']';

            // Print the reversed string without brackets and dots
            printf("Reversed String: ");
            for (int i = 1; i < user_str_len - 1; i++) {  // Skip the first and last characters (brackets)
                if (buff[i] != '.') {  // Skip padding in the printed reversed string
                    putchar(buff[i]);
                }
            }

            putchar('\n');
            break;
        }

        case 'w': {
            printf("Word Print\n----------\n");
            char *start = buff + 1;  // Skip the opening bracket '['
            char *end = buff + 1;    // Start from the first character after '['
            int word_count = 1;
            int total_words = 0;  // To store the total number of words printed

            // Loop through the buffer until we hit the last character before the padding (dots and closing bracket)
            while (*end != ']' && *end != '\0') {  // Stop when we hit the closing bracket ']' or null terminator
                if (*end == ' ' || *end == '.') {
                    // We found a space or a dot (end of word), process the word
                    if (start != end && *start != '.' && *start != ' ') {
                        // Only print the word if it's not padding or empty
                        char temp = *end;
                        *end = '\0';  // Temporarily terminate the string for printing

                        printf("%d. %s (%ld)\n", word_count++, start, end - start);
                        total_words++;  // Increment total word count

                        *end = temp;  // Restore the original character at 'end'
                    }
                    start = end + 1;  // Move the start to the next word
                }
                end++;  // Move to the next character
            }

            // Handle last word after the loop if needed
            if (start != end && *start != '.' && *start != ']') {  // Ensure it's before the padding
                printf("%d. %s(%ld)\n", word_count++,start,end - start);
                total_words++;  // Increment total word count
            }

            // Print the total number of words
            printf("\nNumber of words returned: %d\n", total_words);

            break;
        }

        case 'x': {
            if (argc != 5) {
                fprintf(stderr, "Error: -x requires exactly 3 arguments.\n");
                exit(1); // Return error code
            }
     
            printf("Not Implemented!\n");
            exit(0);
        }
        


        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
//
//
// 1.  Functions that explicitly take a length parameter are more reusable, 
//     as they are not tied to a fixed size (e.g., 50 bytes in this case). 
//     They can be used with buffers of varying sizes without modification.
//
// 2. Even if the buffer size is fixed in main(), passing the length 
//    ensures that the function operates within the defined bounds, preventing potential buffer overflows. 
//    This makes the code safer and more robust.
//
// 3. If the buffer size in main() changes in the future 
//    (e.g., to 100 bytes or dynamically allocated memory), the functions will still work correctly without 
//    requiring changes.
//
// 4. Some functions may need to process only a subset of the buffer. 
//    By passing both the pointer and length, it becomes easier to work with slices of the buffer.
//
// In summary, this approach enhances safety, reusability, flexibility, and clarity, 
// even if the fixed size of buff is known in main().