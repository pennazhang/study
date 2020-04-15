FROM LINUX:VERSION
 
RUN apt-get update && \
	apt-get -y install apt-utils sudo
	
RUN groupadd USER_NAME -g GROUP_ID && \
	useradd -m USER_NAME -u USER_ID -g GROUP_ID && \
	usermod -aG sudo USER_NAME && \
	echo USER_NAME:PASSWORD | chpasswd && \
	echo "USER_NAME ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

ENV SUDO_PASSWD PASSWORD

USER USER_NAME

