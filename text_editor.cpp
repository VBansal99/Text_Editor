#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vector>
#include <string>
#include <fstream>

class UnbufferedInput
{
public:
    // Constructor
    UnbufferedInput()
    {
        // Clear the entire screen
        std::cout << "\033[2J";
        // Move cursor to the top-left position
        std::cout << "\033[1;5H";
        // get the current terminal settings and store into orig_termios
        tcgetattr(STDIN_FILENO, &orig_termios);
        // Now copy those values into new termios
        newtermios = orig_termios;
        // Manipulate those values.
        newtermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG);
        newtermios.c_lflag &= ~(OPOST);
        newtermios.c_lflag &= ~(ICRNL | IXON | INPCK | ISTRIP);
        tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
        drawverticallines('~');
    }
    // Destructor to make the terminal to original state.
    ~UnbufferedInput()
    {
        // Clears the screen
        std::cout << "\033[2J";
        // Move cursor to the top left position
        std::cout << "\033[1;1H";
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
    void enableecho()
    {
        newtermios.c_lflag |= ECHO;
        // newtermios.c_lflag |= (ICRNL | IXON | INPCK | ISTRIP);
        tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
    }
    void disableecho()
    {
        newtermios.c_lflag &= ~ECHO;
        // newtermios.c_lflag &= ~(ICRNL | IXON | INPCK | ISTRIP);
        tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
    }
    void movecursorup(int n = 1)
    {
        std::cout << "\033[" << n << "A";
    }
    void movecursordown(int n = 1)
    {
        std::cout << "\033[" << n << "B";
    }
    void movecursorleft(int n = 1)
    {
        std::cout << "\033[" << n << "D";
    }
    void movecursorright(int n = 1)
    {
        std::cout << "\033[" << n << "C";
    }

private:
    struct termios newtermios, orig_termios;

    void drawverticallines(char c)
    {
        int row_size = 0;
        int col_size = 0;
        struct winsize w;
        if (ioctl(STDIN_FILENO, TIOCGWINSZ, &w) == -1)
        {
            perror("Invalid window size");
            return;
        }

        row_size = w.ws_row;
        for (int i = 0; i < row_size; i++)
        {
            std::cout << c << std::endl;
        }
        std::cout << "\033[1;1H";
    }
};
int main()
{
    // Creates the object of the class
    UnbufferedInput input;
    char c;
    std::cout << "Press any key (OR Press Q to exit)" << std::endl;
    // Get the value from the user until the user press the q button to quit.
    std::string currentline;
    std::vector<std::string> data;
    bool isEditorMode = false;
    std::string filename;
    while (true)
    {
        c = input.getChar();
        if (isEditorMode)
        { // Esc Key is pressed
            if (c == 27)
            {
                c = input.getChar();
                if (c == 'i')
                {
                    std::cout << std::endl;
                    std::cout << "User is in editor mode" << std::endl;
                }
                else
                {
                    isEditorMode = false;
                    input.disableecho();
                    std::cout << "User is in control mode" << std::endl;
                }
            }
            else if (c == '\n')
            {
                data.push_back(currentline);
                currentline.clear();
            }
            else if (c == 19)
            {
                std::cout << "Enter the file name:- " << std::endl;
                std::cin >> filename;
                std::ofstream saveFile(filename);
                if (saveFile.is_open())
                {
                    for (const auto &line : data)
                    {
                        saveFile << line << std::endl;
                    }
                    saveFile.close();
                    std::cout << filename << "saved successfully" << std::endl;
                }
                else
                    std::cerr << "Failed to save file" << std::endl;
            }
            else
            {
                currentline += c;
            }
        }
        else
        {
            if (c == 27)
            {
                c = input.getChar();
                if (c == 'i')
                {
                    isEditorMode = true;
                    input.enableecho();
                    std::cout << "Switched to editor mode and type your text" << std::endl;
                }
            }
            else
            {
                if (c == 'q')
                    break; // Exit command
                // Handle other control commands here
                if (c == 'w')
                    input.movecursorup();
                else if (c == 'a')
                    input.movecursorleft();
                else if (c == 's')
                    input.movecursordown();
                else if (c == 'd')
                    input.movecursorright();
                else
                    std::cout << "Unknown command: " << c << std::endl;
            }
        }
    }
    return 0;
}
