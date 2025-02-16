#!/bin/bash

echo "Testing ish shell..."

# 创建测试目录
mkdir -p test_dir
cd test_dir

# 创建测试文件
echo "Test content" > test.txt

# 启动ish并执行测试命令
{
    echo "ls"
    echo "pwd"
    echo "cd .."
    echo "pwd"
    echo "setenv TEST value1"
    echo "printenv"
    echo "alias ll 'ls -l'"
    echo "ll"
    echo "ls > output.txt"
    echo "cat < output.txt"
    echo "quit"
} | ../ish

# 检查测试结果
if [ -f output.txt ]; then
    echo "Redirection test passed"
else
    echo "Redirection test failed"
fi

cd ..
rm -rf test_dir 