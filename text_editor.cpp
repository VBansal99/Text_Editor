#include <iostream>
#include <termios.h>
#include <unistd.h>
class UnbufferedInput
{
public:
    // Constructor
    UnbufferedInput()
    {
        // get the current terminal settings and store into orig_termios
        tcgetattr(STDIN_FILENO, &orig_termios);
        // Now copy those values into new termios
        newtermios = orig_termios;
        // Manipulate those values.
        newtermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG);
        newtermios.c_lflag &= ~(OPOST);
        newtermios.c_lflag &= ~(ICRNL | IXON | INPCK | ISTRIP);
        tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
    }
    // Destructor to make the terminal to original state.
    ~UnbufferedInput()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    }
    // Getchar functiion to get the value and retun the value
    char getChar()
    {
        char c;
        // STDIN_FILE_NO:- reads the single character from the keyboard
        read(STDIN_FILENO, &c, 1);
        return c;
    }

private:
    struct termios newtermios, orig_termios;
};
int main()
{
    // Creates the object of the class
    UnbufferedInput input;
    char c;
    std::cout << "Press any key (OR Press Q to exit)" << std::endl;
    // Get the value from the user until the user press the q button to quit.
    while (true)
    {
        // Get input from the user
        c = input.getChar();
        std::cout << "You Pressed Key " << c << std::endl;
        if (c == 'q')
            break;
    }
    return 0;
}