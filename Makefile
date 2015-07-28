SUBDIRS=ch17_adv_ipc \
		ch16_socket \
        ch15_inter_proc \
        ch14_adv_IO \
        common

all:
	for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir; \
	done
