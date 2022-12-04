pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
            sh 'sh build.sh'
            }
        }
        stage('Test') {
            steps {
            sh 'pwd'
            sh 'ctest'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}