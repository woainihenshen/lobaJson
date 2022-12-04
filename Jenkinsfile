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
            sh 'cd ${WORKSPACE}/build && test'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}