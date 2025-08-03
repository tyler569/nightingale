class DependencyResolver
  def self.resolve(packages)
    resolved = []
    visited = Set.new
    temp_mark = Set.new
    
    packages.each do |package|
      visit(package, packages, resolved, visited, temp_mark)
    end
    
    resolved
  end
  
  private
  
  def self.visit(package, all_packages, resolved, visited, temp_mark)
    return if visited.include?(package.name)
    
    if temp_mark.include?(package.name)
      raise "Circular dependency detected involving #{package.name}"
    end
    
    temp_mark.add(package.name)
    
    # Visit dependencies first
    package.dependencies.each do |dep_name|
      dep_package = all_packages.find { |p| p.name == dep_name }
      
      unless dep_package
        raise "Dependency not found: #{dep_name} (required by #{package.name})"
      end
      
      visit(dep_package, all_packages, resolved, visited, temp_mark)
    end
    
    temp_mark.delete(package.name)
    visited.add(package.name)
    resolved << package
  end
end