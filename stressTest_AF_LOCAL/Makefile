multiwriter := multiwriter/
massivereader := massivereader/
programs := $(massivereader) $(multiwriter) 

.PHONY: all $(programs) clean
all: $(programs)
	ln -sf ../multiwriter/multiwriter ./ODP/multiwriter
	ln -sf ../massivereader/massivereader ./ODP/massivereader

$(programs):
	$(MAKE) --directory=$@
	
clean: 

