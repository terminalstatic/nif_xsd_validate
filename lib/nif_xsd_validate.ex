defmodule NifXsd do
    @on_load :load_nifs
  
    def load_nifs do
      :ok = :erlang.load_nif(Path.join(:code.priv_dir(:nif_xsd_validate), "nif_xsd_validate"), 0) 
    end
  
    def validate(_resource, _xml) do
      raise "NIF fast_compare/0 not implemented"
    end

    def load_schema(_path) do
        raise "NIF fast_compare/0 not implemented"
      end  
  end