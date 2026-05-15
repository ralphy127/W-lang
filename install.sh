#!/bin/bash

set -e

echo "Building W-lang..."
cd build
cmake ..
cmake --build . --parallel

echo "Installing to $HOME/.local..."
cmake --install . --prefix "$HOME/.local"

cd ..

INSTALL_DIR="$HOME/.local/bin"

echo "----------------------------------------"

if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo "The directory $INSTALL_DIR is not in your PATH."
    read -p "Do you want to automatically add it to your shell configuration? [Y/n] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
        SHELL_RC=""
        if [[ "$SHELL" == *"zsh"* ]]; then
            SHELL_RC="$HOME/.zshrc"
            if [ ! -f "$SHELL_RC" ]; then touch "$SHELL_RC"; fi
        elif [[ "$SHELL" == *"bash"* ]]; then
            if [[ "$OSTYPE" == "darwin"* ]]; then
                SHELL_RC="$HOME/.bash_profile"
            else
                SHELL_RC="$HOME/.bashrc"
            fi
        fi

        if [ -n "$SHELL_RC" ]; then
            echo "" >> "$SHELL_RC"
            echo "# W-lang" >> "$SHELL_RC"
            echo "export PATH=\"$INSTALL_DIR:\$PATH\"" >> "$SHELL_RC"
            echo "Successfully added $INSTALL_DIR to $SHELL_RC."
            echo "Please restart your shell or run: source $SHELL_RC"
        else
            echo "Could not detect your shell configuration file."
            echo "Please manually add the following line to your shell profile:"
            echo "export PATH=\"$INSTALL_DIR:\$PATH\""
        fi
    else
        echo "Skipping PATH configuration."
        echo "You will need to run the executable using its absolute path, e.g.:"
        echo "$INSTALL_DIR/wlang path/to/script.weird"
    fi
else
    echo "W-lang successfully installed!"
    echo "$INSTALL_DIR is already in your PATH."
    echo "You can now run: wlang path/to/script.weird"
fi
