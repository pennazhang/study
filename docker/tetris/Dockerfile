FROM LINUX:VERSION
 
RUN apt-get update && \
	apt-get -y install apt-utils sudo
	
RUN groupadd USER_NAME -g GROUP_ID && \
	useradd -m USER_NAME -u USER_ID -g GROUP_ID && \
	usermod -aG sudo USER_NAME && \
	echo USER_NAME:PASSWORD | chpasswd && \
	echo "USER_NAME ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

RUN apt-get update && apt-get install -y locales && rm -rf /var/lib/apt/lists/* \
    && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8

ENV LANG en_US.utf8

ENV SUDO_PASSWD PASSWORD

USER USER_NAME

