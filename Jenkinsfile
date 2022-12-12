pipeline {
    agent any

    stages {
         stage('检测代码质量') {
             steps {
             echo '检测代码质量'
              }
         }
        stage('Build') {
            steps {
            sh 'sh build.sh'
            }
        }
        stage('Test') {
            steps {
            sh 'cd ${WORKSPACE}/build && ctest'
            }
        }
        stage('checkMemoryLeak') {
            steps {
            sh 'cd ${WORKSPACE}/build/tests && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./lobaJsonTest'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}