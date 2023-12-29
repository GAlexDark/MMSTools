PROJECT_ROOT_DIRECTORY = $${PWD}
TEST_SRCDIR = $${PROJECT_ROOT_DIRECTORY}/test_source/
GIT_VERSION = $$system(git --git-dir $$PROJECT_ROOT_DIRECTORY/.git --work-tree $$PROJECT_ROOT_DIRECTORY describe --always --tags)
