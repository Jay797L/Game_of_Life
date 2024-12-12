clang-format -i *.c
git add *.c
echo "Введите комментарий: "
read comment
git commit -m "$comment"
git push